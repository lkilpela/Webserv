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
		Server(int fd, const ServerConfig& serverConfig);
		~Server();

		int addConnection();
		void closeConnection(int fd);
		void removeConnection(int fd);
		void process(int fd, short& events);
		void sendResponse(int fd, short& events);
		int getServerFd() const;
		std::unordered_map<int, http::Connection>& getAllConnections();

	private:
		int _fd;
		const ServerConfig& _serverConfig;
		Router _router;
		std::unordered_map<int, http::Connection> _connections;

		// void _read(struct ::pollfd& pollFd, http::Connection& con);
		// void _readFromPipe(struct ::pollfd& pollFd, http::Connection& con);
		// void _readFromSocket(struct ::pollfd& pollFd, http::Connection& con);
		// void _handle(struct ::pollfd& pollFd, http::Connection& con);
		// void _cgiHandler(http::Request &req, http::Response &res);
		void _cleanup();
};
