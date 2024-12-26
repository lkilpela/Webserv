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
    // Get the file status flags
    int flags = fcntl(sockfd, F_GETFL, 0); // Assume flags = 0x0000 (O_RDONLY)
    if (flags == -1) {
        throw NetworkError(errno);
    }
    // Set the file status flags
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) { // flags | O_NONBLOCK = 0x0000 | 0x0800 = 0x0800 (O_NONBLOCK)
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

    // Set the receive timeout
    struct timeval timeout = {5, 0};
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == -1) {
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

std::string Server::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        throw FileSystemError(errno);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

HttpResponse createResponse(int statusCode, const std::string& statusMessage, const std::string& message) {
    HttpResponse response;
    response.version = HTTP_1_1;
    response.statusCode = statusCode;
    response.statusMessage = statusMessage;
    response.headers["Content-Type"] = "text/html";
    response.body = message;
    response.headers["Content-Length"] = std::to_string(response.body.size());
    return response;
}

void sendResponse(int clientSockfd, HttpResponse response) {
    std::string rawResponse = response.toString();
    std::cout << "Raw response:\n" << rawResponse << std::endl;
    // why write but not send? because write is a system call to write data to a file descriptor
    // write is used to write data to a file descriptor, send is used to send data to a socket
    // which one is better? write or send? send is better because it is more portable than write
    //write(clientSockfd, rawResponse.c_str(), rawResponse.size());
    // 0 means no flags
    send(clientSockfd, rawResponse.c_str(), rawResponse.size(), 0);
}

void Server::handleRequest(int clientSockfd) {
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
    response.version = HTTP_1_1;

    // Find the matching location block
    const Location* matchedLocation = nullptr;
    for (const auto& location : config.locations) {
        if (request.path.find(location.path) == 0) {
            matchedLocation = &location;
            break;
        }
    }

    if (matchedLocation == nullptr) {
        response = createResponse(HTTP_NOT_FOUND, HTTP_NOT_FOUND_MSG, readFile(errorPages[HTTP_NOT_FOUND]));
        sendResponse(clientSockfd, response);
        return;
    }
    if (request.method == "GET") {
        std::string filePath = fs::canonical(matchedLocation->root + request.path.substr(matchedLocation->path.length())).string();
        if (request.path == matchedLocation->path) {
            filePath = fs::canonical(matchedLocation->root + "/" + matchedLocation->index).string();
        }
        response = createResponse(HTTP_OK, HTTP_OK_MSG, readFile(filePath));
    } else if (request.method == "POST" && matchedLocation->allowUpload) {
        // Handle file upload
        std::string uploadPath = fs::canonical(matchedLocation->uploadDir + "/uploaded_file").string();
        std::ofstream outFile(uploadPath, std::ios::binary);
        outFile << request.body;
        outFile.close();

        response = createResponse(HTTP_OK, HTTP_OK_MSG, "File uploaded successfully");
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

    sendResponse(clientSockfd, response);
}

void Server::handleClient(int clientSockfd) {
    try {
        handleRequest(clientSockfd);
    } catch (const std::system_error& e) {
        std::cout << "System error: " << e.what() << std::endl;
        if(e.code() == std::errc::no_such_file_or_directory) {
            sendResponse(clientSockfd, createResponse(HTTP_NOT_FOUND, HTTP_NOT_FOUND_MSG, errorPages[HTTP_NOT_FOUND]));
        } else {
            sendResponse(clientSockfd, createResponse(HTTP_INTERNAL_SERVER_ERROR, HTTP_INTERNAL_SERVER_ERROR_MSG, errorPages[HTTP_INTERNAL_SERVER_ERROR]));
        }
    } catch (...) {
        sendResponse(clientSockfd, createResponse(HTTP_INTERNAL_SERVER_ERROR, HTTP_INTERNAL_SERVER_ERROR_MSG, errorPages[HTTP_INTERNAL_SERVER_ERROR]));
    }
}

void Server::handleConnections() {
    std::cout << GREEN "Listening for connections " RESET << config.host << ":" << config.port << std::endl;
    std::vector<pollfd> pollfds; // Poll file descriptors
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
