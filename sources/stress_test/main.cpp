#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void)
{
	while (true)
	{

	    int port = 52000;
	    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
	    if (socketFd == -1) {
	        std::cerr << "Socket creation failed" << std::endl;
	        return 1;
	    }

	    struct sockaddr_in serverAddr;
	    serverAddr.sin_family = AF_INET;
	    serverAddr.sin_port = htons(port);
	    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	    if (connect(socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
	        std::cerr << "Connection to server failed" << std::endl;
	        close(socketFd);
	        return 1;
	    }

	    const char *message = "Hello";
	    if (send(socketFd, message, strlen(message) + 1, 0) == -1) {
	        std::cerr << "Send message failed" << std::endl;
	        close(socketFd);
	        return 1;
	    }

	    std::cout << "Port " << port << " is listening..." << std::endl;

	    close(socketFd);
	}
    return 0;
}
