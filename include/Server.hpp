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

		void addClientTo(int serverFd);
		void close(int fd);
		void removeClosedConnections();
		void process(int fd, short& events);
		void sendResponse(int fd, short& events);
		const std::unordered_set<int>& getServerFds() const;
		std::unordered_set<int>& getClosedFds();
		std::unordered_map<int, http::Connection>& getClients();
		std::unordered_map<int, int>& getPipes();

	private:
		const ServerConfig& _serverConfig;
		Router _router;
		std::unordered_set<int> _fds;
		std::unordered_map<int, http::Connection> _connectionByClientFd;
		std::unordered_map<int, int> _pipeFdToClientFd;
		std::unordered_set<int> _closedFds;

		// void _read(struct ::pollfd& pollFd, http::Connection& con);
		// void _readFromPipe(struct ::pollfd& pollFd, http::Connection& con);
		// void _readFromSocket(struct ::pollfd& pollFd, http::Connection& con);
		// void _handle(struct ::pollfd& pollFd, http::Connection& con);
		// void _cgiHandler(http::Request &req, http::Response &res);
		void _cleanup();
};
