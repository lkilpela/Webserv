#include "Config.hpp"
#include <iostream>

class Server {
public:
	Server(const Config& config) : config(config) {}
	void start() {
		for (const auto& serverConfig : config.servers) {
			//std::cout << "Starting server on " << serverConfig.host << ":" << serverConfig.port << std::endl;
            int sockfd = createAndBindSocket(serverConfig);
            // Store the socket file descriptor for later use
            serverSockets.push_back(sockfd);
            // Other server initialization code...
			// Start handling connections
			handleConnections(sockfd);
		}
	}
private:
	const Config& config;
	std::vector<int> serverSockets;

	int createAndBindSocket(const ServerConfig& config);
	void handleClient(int clientSockfd, const ServerConfig& serverConfig);
	void handleConnections(int serverSockfd);
	void sendErrorResponse(int clientSockfd, int statusCode, const std::string& statusMessage, const ServerConfig& serverConfig);
};


