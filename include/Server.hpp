#pragma once

#include "Config.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
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
	
	private:
		std::vector<int> serverFds;
		std::vector<pollfd> pollData;
};
