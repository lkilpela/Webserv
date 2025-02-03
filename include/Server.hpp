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
		const Config& _config;
		Router _router;
		std::unordered_set<int>	_serverFds;
		std::vector<pollfd> _pollfds;
		std::unordered_map<int, http::Connection> _connectionByFd;

		void _addConnection(int serverFd);
		void _read(struct ::pollfd& pollFd, http::Connection& con);
		void _readFromPipe(struct ::pollfd& pollFd, http::Connection& con);
		void _readFromSocket(struct ::pollfd& pollFd, http::Connection& con);
		void _process(struct ::pollfd& pollFd, http::Connection& con);
		void _sendResponse(struct ::pollfd& pollFd, http::Connection& con);
		void _cgiHandler(http::Request &req, http::Response &res);
		void _cleanup();
};
