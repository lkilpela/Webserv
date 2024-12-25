#include "Config.hpp"
#include <iostream>

class Server {
public:
	Server(const ServerConfig& config) : config(config) {}
	void start() {
		serverSockfd = createAndBindSocket();
		handleConnections();
	}
private:
	const ServerConfig& config;
	int serverSockfd;

	int createAndBindSocket();
	void handleClient(int clientSockfd);
	void handleConnections();
	void sendErrorResponse(int clientSockfd, int statusCode, const std::string& statusMessage);
};
