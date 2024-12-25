#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Constant.hpp"
#include "CgiHandler.hpp"
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
#include <fstream> // std::ifstream
#include <sstream> // std::stringstream
#include <filesystem> // fs::canonical
#include <map> // std::map
#include <string> // std::string
#include <vector> // std::vector
#include <sys/types.h> // ssize_t
#include <sys/socket.h> // read, write
#include <errno.h> // errno

namespace fs = std::filesystem;

void setNonBlocking(int sockfd) {
	int flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1) {
		throw NetworkError(errno);
	}
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		throw NetworkError(errno);
	}
}

int Server::createAndBindSocket() {
    std::cout << "Creating and binding socket for server " << config.serverName << std::endl;
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

    std::cout << GREEN "Socket created and bound for " RESET << config.host << ":" << config.port << std::endl;
    return sockfd;
}

void Server::handleClient(int clientSockfd) {
    // Handle data from client
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead = read(clientSockfd, buffer, sizeof(buffer));
    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            std::cout << "Client disconnected" << std::endl;
        } else {
            throw FileSystemError(errno);
        }
    }

    // Process data
    std::string rawRequest(buffer, bytesRead);
    std::cout << BLUE "Raw request:\n" RESET << rawRequest << std::endl; // Debug
    HttpRequest request = HttpRequest::parse(rawRequest);
    if (!request.validate()) {
        throw RequestError(EINVAL);
    }
    std::cout << BLUE "Received request: " RESET << request.method << " " << request.path << std::endl;

    // Generate response
    HttpResponse response;
    response.version = "HTTP/1.1";

    // Find the matching location block
    const Location* matchedLocation = nullptr;
    for (const auto& location : config.locations) {
        if (request.path.find(location.path) == 0) {
            matchedLocation = &location;
            break;
        }
    }

    if (matchedLocation == nullptr) {
        throw RequestError(HTTP_NOT_FOUND);
    }

    if (request.method == "GET") {
        std::string filePath = fs::canonical(matchedLocation->root + request.path.substr(matchedLocation->path.length())).string();
        if (request.path == matchedLocation->path) {
            filePath = fs::canonical(matchedLocation->root + "/" + matchedLocation->index).string();
        }

        std::ifstream file(filePath, std::ios::binary);
        if (file) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            response.statusCode = HTTP_OK;
            response.statusMessage = HTTP_OK_MSG;
            response.headers["Content-Type"] = DEFAULT_CONTENT_TYPE;
            response.body = buffer.str();
            response.headers["Content-Length"] = std::to_string(response.body.size());
        } else {
            throw FileSystemError(errno);
        }
    } else if (request.method == "POST" && matchedLocation->allowUpload) {
        // Handle file upload
        std::string uploadPath = fs::canonical(matchedLocation->uploadDir + "/uploaded_file").string();
        std::ofstream outFile(uploadPath, std::ios::binary);
        outFile << request.body;
        outFile.close();

        response.statusCode = HTTP_OK;
        response.statusMessage = HTTP_OK_MSG;
        response.headers["Content-Type"] = "text/plain";
        response.body = "File uploaded successfully";
        response.headers["Content-Length"] = std::to_string(response.body.size());
    } else if (request.method == "POST" && !matchedLocation->cgiExtension.empty()) {
        // Handle CGI script execution
        CgiHandler cgiHandler;
        response = cgiHandler.executeCgi(request);
    } else {
        throw RequestError(EINVAL);
    }

    if (!response.validate()) {
        std::cerr << "Invalid response" << std::endl;
        throw ResponseError(EINVAL);
    }

    std::string rawResponse = response.toString();
    std::cout << "Raw response:\n" << rawResponse << std::endl;
    write(clientSockfd, rawResponse.c_str(), rawResponse.size());
}


void Server::handleConnections() {
    std::cout << GREEN "Listening for connections " RESET << config.host << ":" << config.port << std::endl;
    std::vector<pollfd> pollfds;
    pollfds.push_back({serverSockfd, POLLIN, 0});

    sockaddr_in clientAddr{};
    socklen_t clientAddrSize = sizeof(clientAddr);

    while (true) {
        int pollCount = poll(pollfds.data(), pollfds.size(), -1);
        if (pollCount == -1) {
            throw NetworkError(errno);
        }

        for (size_t i = 0; i < pollfds.size(); ++i) {
            if (pollfds[i].revents & POLLIN) { // POLLIN is set when there is data to read
                if (pollfds[i].fd == serverSockfd) { // if the server socket is ready means there is a new connection
                    // Accept new connection
                    int clientSockfd = accept(serverSockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);
                    if (clientSockfd == -1) {
                        continue;
                    }
                    setNonBlocking(clientSockfd);
                    pollfds.push_back({clientSockfd, POLLIN, 0}); // Add the client socket to the poll list
                    std::cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << std::endl;
                } else { // Else if the client socket is ready means there is data to read
                    // Handle client data
                    handleClient(pollfds[i].fd);
                    close(pollfds[i].fd);
                    pollfds.erase(pollfds.begin() + i);
                    --i;
                }
            }
        }
    }
}
