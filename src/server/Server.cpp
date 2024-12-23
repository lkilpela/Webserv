#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <iostream> // std::cout, std::endl
#include <sys/socket.h> // socket, bind, listen, accept
#include <netinet/in.h> // sockaddr_in
#include <unistd.h> // close
#include <cstdlib> // exit
#include <cstring> // memset
#include <poll.h> // poll
// inet_addr is deprecated, use inet_pton instead
#include <arpa/inet.h> // inet_pton
#include "Error.hpp" // Error
#include <fcntl.h> // fcntl


void setNonBlocking(int sockfd) {
	int flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1) {
		throw NetworkError(errno);
	}
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		throw NetworkError(errno);
	}
}
int Server::createAndBindSocket(const ServerConfig& config) {
    std::cout << "Creating and binding socket for " << config.host << ":" << config.port << std::endl;
	// Create a socket
	// AF_INET is the address family for IPv4
	// SOCK_STREAM is the type of socket, it provides sequenced, reliable, two-way, connection-based byte streams
	// 0 is the protocol, it is usually set to 0 to use the default protocol
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
		throw NetworkError(errno);
    }
	// Reuse the address
	// opt is the option value, 1 enables the option
	// setsockopt is used to set options for the socket
	// SOL_SOCKET is the level at which the option is defined
	// SO_REUSEADDR allows the socket to bind to an address that is already in use
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		throw NetworkError(errno);
    }
	// Bind the socket
	// sockaddr_in is a structure containing an internet address, used to specify an IPv4 address and port.
	// AF_INET is the address family for IPv4
	// htons converts the port number to network byte order
	// inet_pton converts the host address to binary form, it takes the address family as the first argument
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(config.port);
    if (inet_pton(AF_INET, config.host.c_str(), &serverAddr.sin_addr) <= 0) {
        throw NetworkError(errno);
    }
	// casting: sockaddr_in structure to sockaddr structure because socket API expect a pointer to a sockaddr structure, 
	// which is a generic structur for handling addresses
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		throw NetworkError(errno);
    }
	// Set the socket to non-blocking
	setNonBlocking(sockfd);
	// Listen for incoming connections
	// SOMAXCONN is the maximum number of pending connections
    if (listen(sockfd, SOMAXCONN) == -1) {
		throw NetworkError(errno);
    }

    std::cout << "Socket created and bound for " << config.host << ":" << config.port << std::endl;
    return sockfd;
}
/* void acceptConnection(int sockfd) {
	sockaddr_in clientAddr{};
	socklen_t clientAddrSize = sizeof(clientAddr);
	// Accept a connection
	// accept returns a new socket file descriptor for the accepted connection
	int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);
	if (clientSockfd == -1) {
		throw NetworkError(errno);
	}
	// Set the socket to non-blocking
	setNonBlocking(clientSockfd);
	std::cout << "Accepted connection" << std::endl;
	// Close the socket
	close(clientSockfd);
} */

void Server::handleConnections(int serverSockfd) {
    std::cout << "Listening for connections" << std::endl;
    std::vector<pollfd> pollfds;
    pollfds.push_back({serverSockfd, POLLIN, 0});

    while (true) {
        int pollCount = poll(pollfds.data(), pollfds.size(), -1);
        if (pollCount == -1) {
            throw NetworkError(errno);
        }

        for (size_t i = 0; i < pollfds.size(); ++i) {
            if (pollfds[i].revents & POLLIN) {
                if (pollfds[i].fd == serverSockfd) {
                    // Accept new connection
                    sockaddr_in clientAddr{};
                    socklen_t clientAddrSize = sizeof(clientAddr);
                    int clientSockfd = accept(serverSockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);
                    if (clientSockfd == -1) {
                        throw NetworkError(errno);
                        continue;
                    }
                    setNonBlocking(clientSockfd);
                    pollfds.push_back({clientSockfd, POLLIN, 0});
                    std::cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << std::endl;
                } else {
                    // Handle data from client
                    char buffer[1024];
                    ssize_t bytesRead = read(pollfds[i].fd, buffer, sizeof(buffer));
                    if (bytesRead <= 0) {
                        if (bytesRead == 0) {
                            std::cout << "Client disconnected" << std::endl;
                        } else {
                            std::cerr << "read() failed" << strerror(errno) << std::endl;
                        }
                        close(pollfds[i].fd);
                        pollfds.erase(pollfds.begin() + i);
                        --i;
                    } else {
                        // Process data
                        std::string rawRequest(buffer, bytesRead);
                        std::cout << "Raw request:\n" << rawRequest << std::endl; // Debug
                        HttpRequest request = HttpRequest::parse(rawRequest);
                        if (!request.validate()) {
                            std::cerr << "Invalid request" << std::endl;
                            close(pollfds[i].fd);
                            pollfds.erase(pollfds.begin() + i);
                            --i;
                            continue;
                        }
                        std::cout << "Received request: " << request.method << " " << request.path << std::endl;
                        
                        // Generate response
                        HttpResponse response;
                        response.version = "HTTP/1.1";
                        response.statusCode = 200;
                        response.statusMessage = "OK";
                        response.headers["Content-Type"] = "text/plain";
                        response.body = "Hello, world!";
                        response.headers["Content-Length"] = std::to_string(response.body.size());

                        if (!response.validate()) {
                            std::cerr << "Invalid response" << std::endl;
                            close(pollfds[i].fd);
                            pollfds.erase(pollfds.begin() + i);
                            --i;
                            continue;
                        }

                        std::string rawResponse = response.toString();
                        std::cout << "Raw response:\n" << rawResponse << std::endl;
                        write(pollfds[i].fd, rawResponse.c_str(), rawResponse.size());
                    }
                }
            }
        }
    }
}
