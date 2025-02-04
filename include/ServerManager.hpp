#pragma once

#include <vector>
#include <unordered_set>
#include <poll.h>
#include "Config.hpp"
#include "Server.hpp"

class ServerManager {
	public:
		ServerManager() = default;
		ServerManager(const Config& config);
		~ServerManager();
		void listen();

	private:
		const Config& _config;
		std::vector<Server>	_servers;
		std::vector<struct ::pollfd> _pollfds;
		std::unordered_set<int> _newPollfds;
		std::unordered_set<int> _stalePollfds;

		void _processPollfds();
		Server& _findServer(int fd);
		void _updatePollfds();
};
