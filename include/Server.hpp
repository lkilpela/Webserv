#pragma once

#include "Config.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <algorithm>

class Server {
	public:
		Server() = default;
		Server(const Config& config);
		~Server() = default;
		void makeSocket();
		void listen();
};
