#include <iostream>
#include <list>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <poll.h>
#include <fstream>
#include <filesystem>
#include "../http/HttpRequest.hpp"
#include "../http/HttpResponse.hpp"

#define BUFFER_SIZE 1024


std::string readFileToString(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return content;
}

int main(void)
{
	// 설정 초기화 (추후 Config 파일로 대체)
    std::list<int> ports;
    ports.push_back(51000);
    ports.push_back(52000);
    ports.push_back(53000);
	int backLog = 10;
	struct pollfd pollFds[ports.size()];
	std::string resourcesPath = "../../resources";

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
				std::cout << std::endl << "========== Request ==========" << std::endl << std::endl;
				std::cout << buffer;
				std::cout << std::endl << "=============================" << std::endl << std::endl;
				HttpRequest httpRequest;
				httpRequest.parse(buffer);
				std::string reponseBody = readFileToString(resourcesPath + httpRequest.target);
                const char* response = "HTTP/1.1 200 OK\nContent-Length: 12\n\nHello World!";
                send(clientSocketFd, response, strlen(response), 0);
                close(clientSocketFd);
			}
		}
	}

    return 0;
}
