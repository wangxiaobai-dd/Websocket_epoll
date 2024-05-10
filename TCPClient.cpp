#include <string>
#include <iostream>
#include <memory>
#include <functional>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>

class TCPClient
{
	public:
		
		TCPClient(const std::string& name) : clientName(name)
		{
			epfd_1 = epoll_create(1);
			epfd_2 = epoll_create(1);
		}
		bool connect(const std::string& ip, const unsigned short port)
		{
			sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(-1 == sockfd)
				return false;
			socklen_t size = 64 * 1024;
			::setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
			::setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
			struct sockaddr_in addr;
			bzero(&addr, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = inet_addr(ip.c_str());
			addr.sin_port = htons(port);

			int ret = ::connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
			if(ret == -1)
			{
				std::cout << "CONNECT FAIL" << std::endl;
				shutdown();
				return false;
			}
			
			::fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
			struct epoll_event ev;
			ev.events = EPOLLIN;
			ev.data.fd = STDIN_FILENO;
			epoll_ctl(epfd_1, EPOLL_CTL_ADD, STDIN_FILENO, &ev);

			struct epoll_event ev2;
			ev2.events = EPOLLIN;
			ev2.data.ptr = nullptr;
			epoll_ctl(epfd_2, EPOLL_CTL_ADD, sockfd, &ev2);

			std::cout << "CONNECT SUCCESS" << std::endl;
			return true;
		}
		void run()
		{
			struct epoll_event ev;
			struct epoll_event ev2;
			while(true)
			{
				if(::epoll_wait(epfd_1, &ev, 1, 0) > 0)
				{
					if(ev.events & EPOLLIN)
					{
						char buf[100];
						bzero(buf, sizeof(buf));
						::read(STDIN_FILENO, buf, sizeof(buf) - 1);
						::write(sockfd, buf, strlen(buf));
					}
				}
				if(::epoll_wait(epfd_2, &ev2, 1, 0) > 0)
				{
					if(ev2.events & EPOLLIN)
					{
						char buf[200];
						bzero(buf, sizeof(buf));
						int ret = ::recv(sockfd, buf, sizeof(buf), MSG_NOSIGNAL);
						if(ret == 0)
						{
							std::cout << "server close me" << std::endl;
							::shutdown(sockfd, SHUT_RDWR);
							close(sockfd);
							exit(1);
						}
					}
				}
			}
		}
		void shutdown()
		{
			::close(epfd_1);
			::close(epfd_2);
			epfd_1 = -1;
			epfd_2 = -1;
			if(-1 != sockfd)
			{
				::shutdown(sockfd, SHUT_RDWR);
				::close(sockfd);
				sockfd = -1;
			}
		}
	private:
		std::string clientName;
		int sockfd = -1;
		int epfd_1 = -1;
		int epfd_2 = -1;
};

int main(int agrc, char** argv)
{
	std::unique_ptr<TCPClient> client = std::make_unique<TCPClient>("BowClient");
	if(!client)
		return 0;
	if(!client->connect("122.51.106.92", 8500))
	{
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	client->run();
	client->shutdown();
	
	return 0;
}
