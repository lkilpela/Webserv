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

struct Process {
	int pipeFd;
	int clientFd;
	bool isPipeClosed { false };
	pid_t pid;
};

class Server {
	public:
		Server() = default;
		Server(const ServerConfig& serverConfig);
		~Server();

		void addClientTo(int serverFd);
		void close(int fd);
		void process(int fd, short& events);
		void sendResponse(int fd, short& events);

		const std::unordered_set<int>& getServerFds() const;
		std::unordered_map<int, http::Connection>& getClients();
		std::unordered_map<int, Process>& getPipeProcess();
		void _shutDownServer();

	private:
		const ServerConfig& _serverConfig;
		Router _router;
		std::unordered_set<int> _serverFds;
		std::unordered_map<int, http::Connection> _connectionByClientFd;
		std::unordered_map<int, Process> _processByPipeFd;

		// void _read(struct ::pollfd& pollFd, http::Connection& con);
		// void _readFromPipe(struct ::pollfd& pollFd, http::Connection& con);
		// void _readFromSocket(struct ::pollfd& pollFd, http::Connection& con);
		// void _handle(struct ::pollfd& pollFd, http::Connection& con);
		// void _cgiHandler(http::Request &req, http::Response &res);
		void _closePipeFd(int fd);
		void _cleanup();
};
