#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include <cstring>
#include "sha1.h"
#include "base64.h"

#undef htonll
#define htonll(x) ((1 == htonl(1)) ? (x) : ((uint64_t)htonl((x)&0xFFFFFFFF) << 32) | htonl((x) >> 32))
#undef ntohll
#define ntohll(x) htonll(x)

#define WS_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

std::string str_tolower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
	return str;
}

enum WSState 
{
	WS_PARSING_URI = 0,	// 处理请求行
	WS_PARSING_HEADERS = 1,	// 处理请求头
	WS_VERIFYING_KEY = 2,	// KEY验证
	WS_HAND_SHAKING = 3,	// 握手回应
	WS_TRANSMISSION = 4	// 通信
};

enum HeaderState 
{
	H_START = 0,
	H_KEY = 1,	// KEY
	H_COLON = 2,	// KEY后冒号
	H_SPACE_AFTER_COLON = 3, // KEY后冒号的空格
	H_VALUE = 4,	// VALUE
	H_LINE_CR = 5,	// 键值行 \r
	H_LINE_LF = 6,	// 键值行 \n
	H_END_CR = 7,	// 尾行 \r
	H_END_LF = 8,	// 尾行 \n
};

enum ParseResult
{
	R_WAITING = 0,
	R_ERROR = 1,
	R_SUCCESS = 2
};

enum WSFrameType 
{
	INCOMPLETE_DATA = 0,
	RECV_SEGMENT = 1,
	ERROR = 2,
	SUCCESS = 3,
};

enum WSOpcode
{
	CONTINUE = 0x0,
	TEXT = 0x1,
	BINARY = 0x2,
	CLOSE = 0x8,
	PING = 0x9,
	PONG = 0xA
};

struct WSFlag
{ 
	unsigned char opcode:4, rsv3:1, rsv2:1, rsv1:1, fin:1;
	unsigned char payload_len:7, masked:1;
};

struct WSHttpURI
{
	ParseResult parse(std::string& inBuffer);

	std::string resource;
};

struct WSHttpHeaders
{
	ParseResult parse(std::string& inBuffer);

	std::map<std::string, std::string> headerMap;

	HeaderState headerState = H_START; 

	void printHeaders();

	std::string findValue(const std::string&);
};

struct WSSocket
{
	WSHttpURI uri;
	WSHttpHeaders headers;

	bool parseBuffer(std::string& inBuffer, std::string& outBuffer);

	WSState state = WS_PARSING_URI;

	template <typename Parser>
	bool parse(Parser& parser, std::string& inBuffer, WSState nextState) 
	{
		auto result = parser.parse(inBuffer);
		if(result == ParseResult::R_SUCCESS) 
			state = nextState;
	       	else if(result == ParseResult::R_ERROR)
			return false;
		return true;
	}

	bool handshake(std::string& outBuffer);

	WSFrameType handleMsg(std::string& inBuffer);

	void sendMsg(std::string& outBuffer);

	std::string msgQueue;
	std::string sendQueue;
};

void WSSocket::sendMsg(std::string& outBuffer)
{
	// std::string respond = "Hi, receive your message!";
	std::string respond = sendQueue;

	// outBuffer足够大 不分片
	uint8_t header[14] = {0};
	int headerLen = 2; 

	bool masked = false;
	header[0] = WSOpcode::TEXT | (0x1 << 7); 
	if(masked)
		header[1] = 0x1 << 7;
	size_t len = respond.size();

	if(len < 126)
	{
		header[1] |= (uint8_t)len;
	}
	else if(len < 65536)
	{
		header[1] |= 126;
		*(uint16_t*)(header + 2) = htons((uint16_t)len);
		headerLen += 2;
	}
	else 
	{
		header[1] |= 127;
		*(uint64_t*)(header + 2) = htonll((uint64_t)len);
		headerLen += 8;
	}

	if(masked)
	{
		*(uint8_t*)(header + headerLen) = 1;
		*(uint8_t*)(header + headerLen + 1) = 2;
		*(uint8_t*)(header + headerLen + 2) = 3;
		*(uint8_t*)(header + headerLen + 3) = 4;
		headerLen += 4;
	}

	outBuffer += std::string(reinterpret_cast<char*>(header), headerLen);
	outBuffer += respond;
}

WSFrameType WSSocket::handleMsg(std::string& inBuffer)
{
	int bufferSize = inBuffer.size();
	if(bufferSize < 2)
		return INCOMPLETE_DATA;

	const char* buffer = inBuffer.c_str();

	WSFlag flag;
	std::memcpy(&flag, buffer, 2);

	if((flag.opcode > WSOpcode::BINARY && flag.opcode < WSOpcode::CLOSE) || (flag.opcode > WSOpcode::PONG))
		return ERROR;


	int dataLen = 0;
	int headerLen = 2;

	if(flag.payload_len <= 125)
	{
		dataLen = flag.payload_len;
	}
	else if(flag.payload_len == 126)
	{
		headerLen += 2;
		if(headerLen > bufferSize)
			return INCOMPLETE_DATA;

		dataLen = ntohs(*(uint16_t*)(buffer + 2));
	}
	else if(flag.payload_len == 127)
	{
		headerLen += 8;
		if(headerLen > bufferSize)
			return INCOMPLETE_DATA;

		dataLen = ntohll(*(uint64_t*)(buffer + 2));
	}

	std::cout << "handleMsg opcode:" << (int)flag.opcode << " fin:" << (int)flag.fin << " plen:" << (int)flag.payload_len << " masked:" << (int)flag.masked << " dataLen:" << dataLen << " bufferSize:" << bufferSize << std::endl;

	int totalLen = headerLen + dataLen + (flag.masked ? 4 : 0); 
	if(totalLen > bufferSize)
	{
		std::cout << "INCOMPLETE_DATA, headerLen:" << headerLen << ", dataLen:" << dataLen << ", masked:" << (int)flag.masked << ",bufferSize:" << bufferSize << " maskKey:";

		if(flag.masked && headerLen + 4 <= bufferSize)
		{
			uint8_t maskKey[4] = {0};
			*(uint32_t*)maskKey = *(uint32_t*)(buffer + headerLen);
			std::cout << (int)maskKey[0] << " " << (int)maskKey[1] << " " << (int)maskKey[2] << " " << (int)maskKey[3] << std::endl;
		}

		return INCOMPLETE_DATA;
	}

	uint8_t maskKey[4] = {0};
	unsigned char data[dataLen] = {0};
	std::memcpy(data, buffer + headerLen + (flag.masked ? 4 : 0), dataLen);
	
	if(flag.masked)
	{
		*(uint32_t*)maskKey = *(uint32_t*)(buffer + headerLen);
		for(int i = 0; i < dataLen; ++i)
			data[i] ^= maskKey[i % 4];
	}

	auto result = static_cast<WSFrameType>(flag.opcode);

	msgQueue += std::string(reinterpret_cast<const char*>(data), dataLen);

	inBuffer = inBuffer.substr(totalLen); 

	if(flag.fin)
	{
		std::cout << "handleMsg COMPLETE DATA, size:" << msgQueue.size() << ",maskKey:" << (int)maskKey[0] << std::endl;

		sendQueue = msgQueue;
		msgQueue.clear();

		result = SUCCESS;
	}
	else
       	{
		std::cout << "handleMsg SEGMENT" << ",maskKey:" << (int)maskKey[0] <<std::endl;
		
		result = RECV_SEGMENT;
	}

	std::cout << "Finish handleMsg, leftSize:" << inBuffer.size() << " queueSize:" << msgQueue.size() << " totalLen:" << totalLen << " handerLen:" << headerLen << " dataLen:" << dataLen << " result:" << result << std::endl;

	return result;
}

bool WSSocket::parseBuffer(std::string& inBuffer, std::string& outBuffer)
{
	if(state == WS_PARSING_URI)
	{
		if(!parse(uri, inBuffer, WS_PARSING_HEADERS))
			return false;
	}
	if(state == WS_PARSING_HEADERS)
	{
		if(!parse(headers, inBuffer, WS_VERIFYING_KEY))
			return false;
	}
	if(state == WS_VERIFYING_KEY)
	{
		if(!handshake(outBuffer))
			return false;
	}
	else if(state == WS_TRANSMISSION)
	{
		if(handleMsg(inBuffer) == WSFrameType::SUCCESS)
			sendMsg(outBuffer);
	}
	return true;
}

bool WSSocket::handshake(std::string& outBuffer)
{
	std::string value = headers.findValue("upgrade");
	if(value.empty() || str_tolower(value) != "websocket")
		return false;
	value = headers.findValue("connection");
	if(value.empty() || str_tolower(value) != "upgrade")
		return false;
	value = headers.findValue("sec-websocket-version");
	if(value.empty() || value != "13")
		return false;
	
	std::string secretKey = headers.findValue("sec-websocket-key");
	if(secretKey.empty())
		return false;
	secretKey += WS_KEY;

	SHA1 sha;
	unsigned int digest[5];
	sha.Reset();
	sha << secretKey.c_str();
	sha.Result(digest);
	for(int i = 0; i < 5; i++)
		digest[i] = htonl(digest[i]);
	secretKey = base64_encode(reinterpret_cast<const unsigned char*>(digest), 20);

	std::string respond;
	respond += "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: upgrade\r\nSec-WebSocket-Accept: ";
	respond += secretKey;
	respond += "\r\n\r\n";

	outBuffer += respond;

	state = WS_TRANSMISSION;
	std::cout << "FINISH handshake, return key: " << secretKey << std::endl;

	return true;
}

ParseResult WSHttpURI::parse(std::string& inBuffer)
{
	auto pos = inBuffer.find('\r');	
	if(pos == std::string::npos)
		return R_WAITING;
	std::string requestLine = inBuffer.substr(0, pos);

	if(inBuffer.size() >= pos + 1)
		inBuffer = inBuffer.substr(pos + 1); // exclude \r
	else
		inBuffer.clear();

	if(requestLine.substr(0, 3).compare("GET"))
		return R_ERROR;

	pos = requestLine.find("/ws"); // router
	if(pos == std::string::npos)
		return R_ERROR;

	auto spacePos = requestLine.find(' ', pos);
	if(spacePos == std::string::npos)
		return R_ERROR;

	resource = requestLine.substr(pos, spacePos);
	pos = spacePos + 1;

	pos = requestLine.find("/", pos);
	if(pos == std::string::npos || requestLine.size() - pos <= 3)
		return R_ERROR;
	std::string version = requestLine.substr(pos + 1, 3);
	if(version != "1.1")
		return R_ERROR;

	std::cout << "FINISH WSHttpURI" << std::endl;
	return R_SUCCESS;
}
	
ParseResult WSHttpHeaders::parse(std::string& inBuffer)
{
	int keyBegin = -1, keyEnd = -1, valueBegin = -1, valueEnd = -1;
	int readPos = 0;

	size_t i = 0;
	for(; i < inBuffer.size() && headerState != H_END_LF; ++i)
	{
		switch(headerState)
		{
			case H_START:
				{
					if(inBuffer[i] == '\r' || inBuffer[i] == '\n')
						break;
					headerState = H_KEY;
					keyBegin = i;
					readPos = i;
				}
				break;
			case H_KEY:
				{
					if(inBuffer[i] == '\r' || inBuffer[i] == '\n')
						return R_ERROR;
					if(inBuffer[i] == ':')
					{
						keyEnd = i;
						if(keyEnd - keyBegin <= 0)
							return R_ERROR;
						headerState = H_COLON;
					}
				}
				break;
			case H_COLON:
				{
					if(inBuffer[i] == ' ')
						headerState = H_SPACE_AFTER_COLON;
					else
						return R_ERROR;
				}
				break;
			case H_SPACE_AFTER_COLON:
				{
					headerState = H_VALUE;
					valueBegin = i;
				}
				break;
			case H_VALUE:
				{
					if(inBuffer[i] == '\r')
					{
						headerState = H_LINE_CR;
						valueEnd = i;
						if(valueEnd - valueBegin <= 0)
							return R_ERROR;
					}
					else if(i - valueBegin > 255)
						return R_ERROR;
				}
				break;
			case H_LINE_CR:
				{
					if(inBuffer[i] == '\n')
					{
						headerState = H_LINE_LF;
						std::string key(inBuffer.begin() + keyBegin, inBuffer.begin() + keyEnd);
						std::string value(inBuffer.begin() + valueBegin, inBuffer.begin() + valueEnd);
						headerMap[str_tolower(key)] = value;
						readPos = i;
						keyBegin = keyEnd = valueBegin = valueEnd = -1;
					}
					else
						return R_ERROR;
				}
				break;
			case H_LINE_LF:
				{
					if(inBuffer[i] == '\r')
						headerState = H_END_CR;
					else
					{
						headerState = H_KEY;
						keyBegin = i;
					}
				}
				break;
			case H_END_CR:
				{
					if(inBuffer[i] == '\n')
						headerState = H_END_LF;
					else
						return R_ERROR;
				}
				break;
			default:
				break;
		}
	}
	if(headerState == H_END_LF)
	{
		std::cout << "FINISH WSHttpHeaders" << std::endl;
		inBuffer = inBuffer.substr(i);

		std::cout << "InBuffer:" << inBuffer << std::endl;

		// printHeaders();

		return R_SUCCESS;
	}
	else
		inBuffer = inBuffer.substr(readPos);

	return R_WAITING;
}

void WSHttpHeaders::printHeaders()
{
	std::cout << "Headers:" << std::endl;
	for(const auto& [key, value] : headerMap)
	{
		std::cout << key << " : " << value << std::endl;
	}
}

std::string WSHttpHeaders::findValue(const std::string& header)
{
	auto iter = headerMap.find(header);
	if(iter != headerMap.end())
		return iter->second;
	return "";
}
