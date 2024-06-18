#ifndef SERVER_HPP
# define SERVER_HPP

# define BUFFER_SIZE 1024

# include <list>
# include <string>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <cstring>
# include <poll.h>
# include <fstream>
# include <filesystem>
# include <iostream>
# include "../http/HttpRequest.hpp"
# include "../http/HttpResponse.hpp"

class Server {
    public:
        Server(const std::list<int>& ports, const std::string& resourcesPath);
        Server(const Server& obj);
        ~Server();

        Server& operator= (const Server& rhs);

        void start();

    private:
        Server();

        void setupSockets();
        void eventLoop();
        void handleEvent(int serverSocketFd);

        std::list<int> ports_;
        std::string resourcesPath_;
        std::vector<pollfd> pollFds_;
        static const int backLog_;
};

# include "Server.cpp"

#endif
