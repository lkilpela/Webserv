#pragma once

#define BACKLOG 128

#include "Config.hpp"
#include <iostream>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <algorithm>
#include <functional>
#include <poll.h>
#include "http/Connection.hpp"

class Server {
	public:
		Server() = default;
		Server(const Config& config);
		~Server();
		void listen();

	private:
		std::vector<int> _serverFds;
		std::vector<pollfd> _pollfds;
		std::unordered_map<int, http::Connection> _connectionByFd;

		void _addConnection(int fd);
		void _cleanup();
};
