#include <iostream>
#include <list>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <poll.h>
#include "../http/HttpRequest.hpp"

#define BUFFER_SIZE 1024

int main(void)
{
	// 설정 초기화 (추후 Config 파일로 대체)
    std::list<int> ports;
    ports.push_back(51000);
    ports.push_back(52000);
    ports.push_back(53000);
	int backLog = 10;
	struct pollfd pollFds[ports.size()];
	std::string resourcesPath = "../../resources/";

   // 각 포트에 대해 소켓 생성 및 바인딩
   int index = 0;
    for (std::list<int>::iterator it = ports.begin(); it != ports.end(); ++it) 
	{
        int serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);

		sockaddr_in serverAddress;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(*it);
		serverAddress.sin_addr.s_addr = INADDR_ANY;
        
		bind(serverSocketFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
		listen(serverSocketFd, backLog);

		pollFds[index].fd = serverSocketFd;
        pollFds[index].events = POLLIN;
        pollFds[index].revents = 0;
		index++;

		std::cout << "Port " << *it << " is listening..." << std::endl;
    }

	while (true)
	{
		std::cout << "Polling..." << std::endl;
		poll(pollFds, ports.size(), -1);
		for (int i = 0; i < ports.size(); i++)
		{
			if (pollFds[i].revents & POLLIN)
			{
                int clientSocketFd = accept(pollFds[i].fd, nullptr, nullptr);
                char buffer[BUFFER_SIZE];
                int size = recv(clientSocketFd, buffer, BUFFER_SIZE - 1, 0);
                buffer[size] = '\0';
				std::cout << std::endl << "========== Request ==========" << std::endl << std::endl;;
				std::cout << buffer;
				std::cout << std::endl << "=============================" << std::endl << std::endl;;
				HttpRequest request;
				request.parse(buffer);
				std::cout << "Method: " << request.method << std::endl;
				std::cout << "Target: " << request.target << std::endl;
				std::cout << "Version: " << request.version << std::endl;
				std::cout << "Headers: " << std::endl;
				for (std::map<std::string, std::string>::iterator it = request.headers.begin(); it != request.headers.end(); ++it)
				{
					std::cout << it->first << ": " << it->second << std::endl;
				}
				std::cout << "Body: " << request.body << std::endl;
				const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title>Test</title></head><body><h1>Test</hh1></body></html>";
                send(clientSocketFd, response, strlen(response), 0);
                close(clientSocketFd);
			}
		}
	}

    return 0;
}
