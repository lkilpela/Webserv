#pragma once

#define BACKLOG 128

#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <poll.h>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

#include "Config.hpp"
#include "http/index.hpp"
#include "Router.hpp"

struct WorkerProcess {
	enum Status : short {
		RUNNING,
		PENDING_TERMINATION,
		TERMINATED
	};

	WorkerProcess() = default;
	WorkerProcess(const WorkerProcess&) = default;
	WorkerProcess& operator=(const WorkerProcess&) = default;

	int pipeFds[2];
	int clientFd;
	Status status = { RUNNING };
	pid_t pid;
	std::filesystem::path rootPath;
};

class Server {
	public:
		Server() = delete;
		Server(const ServerConfig& serverConfig);
		~Server();

		Server(Server&&) noexcept = default;
		Server& operator=(Server&&) noexcept = default;

		void addCgiHandler();
		void addConnection(int serverFd);
		void closeConnection(http::Connection& con);
		void close(int fd);
		void process(int fd, short& events);
		void sendResponse(int fd, short& events);

		const std::unordered_set<int>& getServerFds() const;
		std::unordered_map<int, http::Connection>& getManagedConnections();
		std::unordered_map<int, WorkerProcess>& getWorkerProcesses();
		void shutdown();

	private:
		const ServerConfig& _serverConfig;
		Router _router;
		std::unordered_set<int> _serverFds;
		std::unordered_map<int, http::Connection> _connections;
		std::unordered_map<int, WorkerProcess> _workerProcesses;

		void _handleCGI(
			const Location& loc,
			const std::string& requestPath,
			const http::Request& request,
			http::Response& response
		);
		// void _read(struct ::pollfd& pollFd, http::Connection& con);
		// void _readFromPipe(struct ::pollfd& pollFd, http::Connection& con);
		// void _readFromSocket(struct ::pollfd& pollFd, http::Connection& con);
		// void _handle(struct ::pollfd& pollFd, http::Connection& con);
		// void _cgiHandler(http::Request &req, http::Response &res);
		void _processConnection(http::Connection& con, short& events);
		void _processWorkerProcess(WorkerProcess& process, short& events);
		void _closePipeFd(int fd);
		void _cleanup();
};
