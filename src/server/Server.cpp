#include "Server.hpp"

void closeFds(std::vector<int> fds){
    std::for_each(fds.begin(), fds.end(), [](int fd) {close(fd);});
}

std::vector<int> makeSocket(const std::vector<int>& ports) {
    std::vector<int> sockets;
    for (int port : ports) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
			closeFds(sockets);
            throw std::runtime_error("Failed to create socket");
        }
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        if (bind(sock, (struct sockaddr*)&address, sizeof(address)) == -1) {
            closeFds(sockets);
            throw std::runtime_error("Failed to bind socket on port " + std::to_string(port));
        }
        if (listen(sock, BACKLOG) < 0)
            closeFds(sockets);
            throw std::runtime_error("Failed to listen on port " + std::to_string(port));
        sockets.push_back(sock);
    }
    return sockets;
}

void acceptAndRepsond(std::vector<int> serverFds){
}

int main() {

	Server sockets;
    try {
        std::vector<int> socketsArray = makeSocket(sockets.getPorts());
        for (int sock : socketsArray) {
            close(sock);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

