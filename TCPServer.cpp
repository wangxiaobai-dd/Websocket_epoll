#include <string>
#include <iostream>
#include <memory>
#include <csignal>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <map>
#include "WSRequest.h"

static int setNonBlocking(int fd)
{
	int flags;
	int nodelay = 1;
	if(::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&nodelay, sizeof(nodelay)))
		return -1;
	flags = ::fcntl(fd, F_GETFL, 0);
	if (-1 == flags) 
		return -1;
	flags |= O_NONBLOCK;
	if(-1 == ::fcntl(fd, F_SETFL, flags))
		return -1;
	return 0;
}

struct ConnData
{
	ConnData(int _fd) : fd(_fd){};

	int fd = -1;
	__uint32_t events;
	
	bool close = false;

	std::string inBuffer;
	std::string outBuffer;

	WSSocket ws;

	void parseBuffer();
};

void ConnData::parseBuffer()
{
	if(!ws.parseBuffer(inBuffer, outBuffer))
		close = true;
}

#define MAXEVENTS 100

class TCPServer
{
	public:
		TCPServer(const std::string& name):serverName(name)
		{
			epfd = epoll_create(MAXEVENTS);
			events = std::vector<epoll_event>(MAXEVENTS);
		}
		~TCPServer()
		{
			TEMP_FAILURE_RETRY(::close(epfd));
			if(listenfd != -1)
			{
				::shutdown(listenfd, SHUT_RD);
				TEMP_FAILURE_RETRY(::close(listenfd));
				listenfd = -1;
			}
		}
		TCPServer(const TCPServer&) = delete;
		TCPServer& operator = (const TCPServer&) = delete;
		
		bool bind(const unsigned short port);
		void serverEpollAdd(int fd, __uint32_t events);
		void serverEpollClose(int fd);
		int serverEpollWait();
		int handleRead(int fd);
		int handleWrite(int fd);
		void handleConn(int fd);
		void handleEvents(int num);
		void shutdown();

	private:
		std::string serverName;
		int listenfd = -1;
		int epfd = -1;
		std::vector<epoll_event> events;
		std::map<int, std::shared_ptr<ConnData>> dataMap;
};

bool TCPServer::bind(const unsigned short port)
{
	listenfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(-1 == listenfd)
	{
		std::cerr << "SOCKET FAIL" << std::endl;
		return false;	
	}
	socklen_t size = 64 * 1024;
	::setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
	::setsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
	int reuse = 1;
	::setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if(-1 == ::bind(listenfd, (struct sockaddr *)&addr, sizeof(addr)))
	{
		std::cerr << "BIND FAIL" << std::endl;
		shutdown();
		return false;
	}

	if(-1 == ::listen(listenfd, MAXEVENTS))
	{
		std::cerr << "LISTEN FAIL" << std::endl;
		return false;
	}

	serverEpollAdd(listenfd, EPOLLIN);

	std::cout << "BIND SUCCESS, FD:" << listenfd << std::endl;
	return true;
}

void TCPServer::serverEpollAdd(int fd, __uint32_t events)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev); // not resolve error

	auto dataPtr = std::make_shared<ConnData>(fd);
	dataPtr->events = events;
	dataMap[fd] = dataPtr;
}

void TCPServer::serverEpollClose(int fd)
{
	dataMap[fd]->close = true;
}

int TCPServer::serverEpollWait()
{
	int rc = epoll_wait(epfd, &*events.begin(), events.size(), -1);
	if(rc < 0)
	{
		std::cerr << "EPOLL_WAIT ERROR, " << errno << std::endl; 
	}
	return rc;
}

int TCPServer::handleRead(int fd)
{
	auto iter = dataMap.find(fd);
	if(iter == dataMap.end())
	{
		std::cerr << "HANDLE READ NO FD:" << fd << std::endl;
		abort();
		return 0;
	}
	char buff[1024000] = {0};			
	int ret = TEMP_FAILURE_RETRY(::recv(fd, buff, sizeof(buff), MSG_NOSIGNAL));
	if(ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
		return 0;
	if(ret == 0) // close
		return -1;
	if(ret > 0)
	{
		// std::cout << "HANDLE READ, FD:" << fd << " RECV:\n" << buff << "SIZE:" << ret << std::endl;
		iter->second->inBuffer += std::string(buff, buff + ret);
		iter->second->parseBuffer();
	}
	return ret;
}

int TCPServer::handleWrite(int fd)
{
	auto iter = dataMap.find(fd);
	if(iter == dataMap.end())
	{
		std::cerr << "HANDLE WRITE NO FD:" << fd << std::endl;
		abort();
		return 0;
	}
	if(iter->second->outBuffer.empty())
		return 0;
	std::string& buffer = iter->second->outBuffer;
	int ret = ::send(fd, buffer.data(), buffer.size(), MSG_NOSIGNAL); // -1 close
	if(ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
		return 0;
	if(ret > 0)
		buffer = buffer.substr(ret);
	return ret;
}

void TCPServer::handleConn(int fd)
{
	if(dataMap[fd]->close)
	{
		struct epoll_event ev;
		ev.events = dataMap[fd]->events;
		ev.data.fd = fd;
		epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);

		dataMap.erase(fd);

		std::cout << "CLOSE, FD:" << fd << std::endl << std::endl;

		::shutdown(fd, SHUT_RDWR);
		close(fd);
	}
}

void TCPServer::handleEvents(int num)
{
	for(int i = 0; i < num; ++i)
	{
		if(events[i].data.fd == listenfd)
		{
			struct sockaddr_in client_addr;
			memset(&client_addr, 0, sizeof(struct sockaddr_in));
			socklen_t len = sizeof(client_addr);

			int newConnFd = TEMP_FAILURE_RETRY(::accept(listenfd, (struct sockaddr*)&client_addr, &len));
			std::cout << "ACCEPT NEW CONN, FD:" << newConnFd << std::endl;

			setNonBlocking(newConnFd);

			serverEpollAdd(newConnFd, EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLPRI);
		}
		else
		{
			if(events[i].events & EPOLLIN)
			{
				if(handleRead(events[i].data.fd) == -1)
				{
					std::cout << "READ ERR, CLOSE CONN, FD:" << events[i].data.fd << std::endl;
					serverEpollClose(events[i].data.fd);
				}
			}
			if(events[i].events & EPOLLOUT)
			{
				if(handleWrite(events[i].data.fd) == -1)
				{
					std::cout << "WRITE ERR, CLOSE CONN, FD:" << events[i].data.fd << std::endl;
					serverEpollClose(events[i].data.fd);
				}
			}
		}

		events[i].events = 0;

		handleConn(events[i].data.fd);
	}

}

void TCPServer::shutdown()
{
	TEMP_FAILURE_RETRY(::close(epfd));
	if(-1 != listenfd)
	{
		::shutdown(listenfd, SHUT_RD);
		TEMP_FAILURE_RETRY(::close(listenfd));
	}
}

namespace
{
	std::function<void(int)> shutdown_handler;
	void signal_handler(int signal) { shutdown_handler(signal); }
}

int main(int argc, char** argv)
{
	bool exitFlag = false;
	std::unique_ptr<TCPServer> server = std::make_unique<TCPServer>("Server");
	if(!server)
		return 0;
	if(!server->bind(8500))
		return 0;
	// daemon(1,1);

	shutdown_handler = [&exitFlag](int sig){ exitFlag = true; std::cout << "SHUT DOWN" << std::endl;};
	std::signal(SIGINT, signal_handler);

	// one thread
	while(!exitFlag)
	{
		int ret = server->serverEpollWait();
		if(ret > 0)
		{
			server->handleEvents(ret);
		}
		else if(ret < 0)
		{
			std::cout << "epoll err" << std::endl;
		}
	}
	server->shutdown();
	
	return 0;
}
