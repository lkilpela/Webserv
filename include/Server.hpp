#include "Config.hpp"
#include <iostream>

#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"
#define RESET "\033[0m"

class Server {
public:
	Server(const ServerConfig& config) : config(config) {
		// Check if error pages in the config specify
		for(const auto& [code, path] : config.errorPages) {
			errorPages[code] = readFile(path);
		}
	}
	void start() {
		serverSockfd = createAndBindSocket();
		handleConnections();
	}
private:
	const ServerConfig& config;
	int serverSockfd;
	std::map<int, std::string> errorPages;

	std::string readFile(const std::string& filePath);
	int createAndBindSocket();
	void handleRequest(int clientSockfd);
	void handleClient(int clientSockfd);
	void handleConnections();
	//void sendErrorResponse(int clientSockfd, int statusCode, const std::string& statusMessage);
};
