#pragma once

#define BACKLOG 128

#include "Config.hpp"
#include <iostream>
#include <unordered_map>
#include <cstring>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <poll.h>
#include "http/Connection.hpp"
#include "Router.hpp"

class Server {
	public:
		Server() = default;
		Server(const Config& config);
		~Server();
		void listen();

	private:
		const Config &_config;
		Router _router;
		std::unordered_set<int>	_serverFds;
		std::vector<pollfd> _pollfds;
		std::unordered_map<int, http::Connection> _connectionByFd;
		std::unordered_map<int, http::Connection &> _connectionByPipeFd;

		bool _addConnection(int fd);
		bool _read(struct ::pollfd& pollFd, http::Connection& con);
		bool _process(struct ::pollfd& pollFd, http::Connection& con);
		void _sendResponse(struct ::pollfd& pollFd, http::Connection& con);
		void _cleanup();

		std::vector<pollfd>::iterator _removeConnection(
			std::vector<pollfd>::const_iterator it,
			http::Connection& con
		);
};
