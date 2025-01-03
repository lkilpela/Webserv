#pragma once

#include "Config.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <algorithm>
#include <functional>
#include <poll.h>
#include "http/Connection.hpp"
#define BACKLOG 128

class Request;
class Response;

class Server {
	public:
		Server() = default;
		Server(const Config& config);
		~Server();
		void listen();
		void processCGI(Request& req);
		void processHttpClient(Request& req, Response& res);

	private:
		std::vector<int> _serverFds;
		std::vector<pollfd> _pollfds;
		std::unordered_map<int, std::pair<Request, Response>> _requestResponseByFd;
		std::vector<int> _clientFds;
		std::unordered_map<int, http::Connection> _connectionByFd;

		void _addConnection(int fd);
		void _addRoute(std::string, std::function<void(Request& req, Response& res)>);
};
