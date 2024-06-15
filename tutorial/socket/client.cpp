#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h>
#include <unistd.h>  

const int PORT = 3000;

int	main(void)
{
	// 클라이언트 소켓 생성
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket < 0) {
        std::cerr << "Socket Creation Failed: " << strerror(errno) << std::endl;
        return 1;
    }

	std::cout << "ClientSocket Created" << std::endl;

	// 주소 생성
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = PORT;
	addr.sin_addr.s_addr = INADDR_ANY;

	// 서버 소켓에 연결 요청
	if (connect(clientSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "Server Connection Failed: " << strerror(errno) << std::endl;
        close(clientSocket);
        return 1;
	}

	std::cout << "ServerSocket Connected" << std::endl;

	// 연결된 서버 소켓에 데이터 전송
	const char* message = "Hello, webserv!";
	if (send(clientSocket, message, strlen(message), 0) < 0)
	{
		std::cerr << "Data Sending Failed: " << strerror(errno) << std::endl;
        close(clientSocket);
        return 1;
	}

	std::cout << "Message sent to the server: " << message << std::endl;

	// 클라이언트 소켓 종료
	if (close(clientSocket) < 0) {
        std::cerr << "Socket Closing Failed: " << strerror(errno) << std::endl;
        return 1;
    }

	std::cout << "Socket Closed" << std::endl;

	return 0;
}