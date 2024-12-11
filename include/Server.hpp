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

class Server {
	public:
		Server() = default;
		// Server(const Config& config);
		Server(const std::vector<int> ports);
		~Server();
		void listen();
	
	private:
		std::vector<int> _serverFds;
		std::vector<pollfd> _pollData;
		std::vector<int> _clientFds;
};
