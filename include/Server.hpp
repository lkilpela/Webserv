#pragma once

#include "Config.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <algorithm>
#include <poll.h>
#define BACKLOG 128

class Request;
class Response;

class Server {
	public:
		Server() = default;
		// Server(const Config& config);
		Server(const std::vector<int> ports);
		~Server();
		void listen();
		void process(Request& req, Response& res);

	private:
		std::vector<int> _serverFds;
		std::vector<pollfd> _pollfds;
		std::unordered_map<int, std::pair<Request, Response>> _requestResponseByFd;
		std::vector<int> _clientFds;

		void _addClient(std::size_t i);
};
