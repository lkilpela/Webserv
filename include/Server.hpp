#include "Config.hpp"
#include <iostream>

#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"
#define RESET "\033[0m"

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
	//void sendErrorResponse(int clientSockfd, int statusCode, const std::string& statusMessage);
};
