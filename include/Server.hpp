#pragma once

#include "Config.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <poll.h>
#define BACKLOG 128

#include <cstring>

struct Request {
    string method;
    string path;
    string host;
    unsigned int port;
};

class Server {
	public:
		Server() = default;
		// Server(const Config& config);
		Server(const std::string& host, int port)
		: host(host)
		, port(port)
		{
			std::cout << "Initialzing server at " << host << ":" << port << std::endl;
		}
		Server(const std::vector<int> ports);
		~Server();
		void listen();
		void start();
		void handleRequest(const Request& request, const Config& config);

	
	private:
		std::vector<int> _serverFds;
		std::vector<pollfd> _pollData;
		std::vector<int> _clientFds;

		std::string host;
		int port;
};
