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

class Servers {
	public:
		Servers() = default;
		// Server(const Config& config);
		Servers(const std::vector<int> ports);
		~Servers();
		void pollServers(Servers&);
	
	private:
		std::vector<int> serverFds;
		std::vector<pollfd> pollData;
		std::vector<int> clientFds;
};
