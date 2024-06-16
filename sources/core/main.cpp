#include <iostream>
#include <list>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <poll.h>
#include <fstream>
#include <filesystem>
#include <thread>
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

bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) return false;
    return str.substr(str.length() - suffix.length()) == suffix;
}

void workerProcess( pollfd *pollFds, int portSize, std::string& resourcesPath)
{
	while (true)
	{
		std::cout << "Polling..." << std::endl;
		poll(pollFds, portSize, -1);
		for (int i = 0; i < portSize; i++)
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
				std::string responseBody = readFileToString(resourcesPath + httpRequest.target);
				if (endsWith(httpRequest.target, ".svg"))
				{
					std::string response = "HTTP/1.1 200 OK\nContent-Type: image/svg+xml\nContent-Length: " + std::to_string(responseBody.length()) + "\n\n"
                      + responseBody;
                	send(clientSocketFd, response.c_str(), strlen(response.c_str()), 0);
                	close(clientSocketFd);
				}
				else
				{
					std::string response = "HTTP/1.1 200 OK\nContent-Length: " + std::to_string(responseBody.length()) + "\n\n"
                      + responseBody;
                	send(clientSocketFd, response.c_str(), strlen(response.c_str()), 0);
                	close(clientSocketFd);
				}
				
			}
		}
	}
}

int main(void)
{
	// 설정 초기화 (추후 Config 파일로 대체)
    std::list<int> ports;
    ports.push_back(51000);
    ports.push_back(52000);
    ports.push_back(53000);
	int backLog = 10;
	struct pollfd pollFds[3];
	std::string resourcesPath = "../../resources";
	int numOfWorkers = 4;

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


	
	std::thread t1(workerProcess, pollFds, ports.size(), resourcesPath);
	std::thread t2(workerProcess, pollFds, ports.size(), resourcesPath);
	std::thread t3(workerProcess, pollFds, ports.size(), resourcesPath);
	std::thread t4(workerProcess, pollFds, ports.size(), resourcesPath);
	

    return 0;
}
