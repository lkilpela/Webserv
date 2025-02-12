#pragma once

#include <vector>
#include <functional>
#include <unordered_set>
#include <poll.h>
#include "Config.hpp"
#include "Server.hpp"

class ServerManager {
	public:
		ServerManager() = delete;
		ServerManager(const Config& config);
		~ServerManager() = default;
		void listen();

	private:
		const Config& _config;
		std::vector<Server>	_servers;
		std::vector<struct ::pollfd> _pollfds;
		std::unordered_map<int, std::reference_wrapper<Server>> _serverMap;
		std::unordered_set<int> _newFds;
		std::unordered_set<int> _staleFds;

		void _processPollfds();
		void _pruneClosedConnections();
		void _updatePollfds();
};
