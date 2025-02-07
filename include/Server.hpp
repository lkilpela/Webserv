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
		Server(const ServerConfig& serverConfig);
		~Server();

		int addConnection(int serverFd);
		void closeConnection(int fd);
		void removeConnection(int fd);
		void process(int fd, short& events);
		void sendResponse(int fd, short& events);
		const std::unordered_set<int>& getFds() const;
		std::unordered_map<int, http::Connection>& getConnectionMap();

	private:
		std::unordered_set<int> _fds;
		const ServerConfig& _serverConfig;
		Router _router;
		std::unordered_map<int, http::Connection> _connectionMap;

		// void _read(struct ::pollfd& pollFd, http::Connection& con);
		// void _readFromPipe(struct ::pollfd& pollFd, http::Connection& con);
		// void _readFromSocket(struct ::pollfd& pollFd, http::Connection& con);
		// void _handle(struct ::pollfd& pollFd, http::Connection& con);
		// void _cgiHandler(http::Request &req, http::Response &res);
		void _cleanup();
};
