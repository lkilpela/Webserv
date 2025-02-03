#pragma once

#include <vector>
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
};
