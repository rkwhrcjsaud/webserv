#include <list>
#include "Server.hpp"

int main(void)
{
	// 설정 초기화 (추후 Config 파일로 대체)
    std::list<int> ports;
    ports.push_back(51000);
    ports.push_back(52000);
    ports.push_back(53000);
	Server server(ports, "../../resources");

	server.start();

    return 0;
}
