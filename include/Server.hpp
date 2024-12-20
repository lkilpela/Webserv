#include "Config.hpp"
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

int createAndBindSocket(const ServerConfig& config) {
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
    serverAddr.sin_addr.s_addr = inet_pton(AF_INET, config.host.c_str(), &serverAddr.sin_addr);

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

    return sockfd;
}

void acceptConnection(int sockfd) {
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
}
