#pragma once

#include <vector>
#include <functional>
#include <unordered_set>
#include <poll.h>
#include "Config.hpp"
#include "Server.hpp"

class ServerManager {
	public:
		ServerManager() = default;
		ServerManager(const Config& config);
		~ServerManager() = default;
		void listen();

	private:
		const Config& _config;
		std::vector<Server>	_servers;
		std::vector<struct ::pollfd> _pollFds;
		std::unordered_map<int, std::size_t> _pollFdIndexByFd;
		std::unordered_map<int, std::reference_wrapper<Server>> _serverByFd;

		void _shutDownServers();
		void _addPollFd(int fd, Server& server);
		void _removePollFd(int fd);
		void _updatePollFds();
		void _processPollFds();
		void _updateClientConnections(Server& server);
		void _updatePipeConnections(Server& server);
};
