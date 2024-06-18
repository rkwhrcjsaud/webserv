#include "Server.hpp"

const int Server::backLog_ = 10;

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

Server::Server(const std::list<int>& ports, const std::string& resourcesPath)
    : ports_(ports), resourcesPath_(resourcesPath) {
    pollFds_.resize(ports_.size());
}

Server::Server(const Server& obj)
    : ports_(obj.ports_), resourcesPath_(obj.resourcesPath_), pollFds_(obj.pollFds_) {
}

Server::Server()
    : ports_(), resourcesPath_() {
}

Server::~Server() {
}

Server& Server::operator= (const Server& rhs) {
    ports_ = rhs.ports_;
    resourcesPath_ = rhs.resourcesPath_;
    pollFds_ = rhs.pollFds_;
    return *this;
}

void Server::start() {
    setupSockets();
    eventLoop();
}

void Server::setupSockets() {
    int index = 0;
    for (std::list<int>::iterator it = ports_.begin(); it != ports_.end(); ++it) {
        int port = *it;
        int serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        bind(serverSocketFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        listen(serverSocketFd, backLog_);

        pollFds_[index].fd = serverSocketFd;
        pollFds_[index].events = POLLIN;
        pollFds_[index].revents = 0;
        index++;

        std::cout << "Port " << port << " is listening..." << std::endl;
    }
}

void Server::eventLoop() {
    while (true) {
        std::cout << "Polling..." << std::endl;
        poll(pollFds_.data(), pollFds_.size(), -1);
        for (int i = 0; i < pollFds_.size(); i++) {
            if (pollFds_[i].revents & POLLIN) {
                handleEvent(pollFds_[i].fd);
            }
        }
    }
}

void Server::handleEvent(int serverSocketFd) {
    int clientSocketFd = accept(serverSocketFd, nullptr, nullptr);
    char buffer[BUFFER_SIZE];
    int size = recv(clientSocketFd, buffer, BUFFER_SIZE - 1, 0);
    buffer[size] = '\0';

    std::cout << std::endl << "========== Request ==========" << std::endl << std::endl;
    std::cout << buffer;
    std::cout << std::endl << "=============================" << std::endl << std::endl;

    HttpRequest httpRequest;
    httpRequest.parse(buffer);

    std::string responseBody = readFileToString(resourcesPath_ + httpRequest.target);
    std::string response = "";
    if (endsWith(httpRequest.target, ".svg"))
    {
        response = "HTTP/1.1 200 OK\r\nContent-Type: image/svg+xml\r\nContent-Length: "
            + std::to_string(responseBody.length()) + "\r\n\r\n"
            + responseBody;
    }
    else
    {
        response = "HTTP/1.1 200 OK\r\nContent-Length: "
            + std::to_string(responseBody.length()) + "\r\n\r\n"
            + responseBody;
    }
    send(clientSocketFd, response.c_str(), response.size(), 0);
    close(clientSocketFd);
}
