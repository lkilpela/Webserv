#include <sys/socket.h>
#include "ServerManager.hpp"
#include "utils/index.hpp"

ServerManager::ServerManager(const Config& config) : _config(config) {
	_servers.reserve(config.servers.size());

	for (std::size_t i = 0; i < config.servers.size(); i++) {
		const int port = config.ports[i];
		const ServerConfig serverConfig = config.servers[i];

		int serverFd = utils::createPassiveSocket(port, 128, true);
		_servers.push_back(Server(serverFd, serverConfig));
		_serverMap.emplace(serverFd, _servers.back());
		_pollfds.push_back({ serverFd, POLLIN, 0 });
    }
}

void ServerManager::listen() {
	while (_pollfds.size()) {
        int ret = ::poll(_pollfds.data(), _pollfds.size(), 100);

		if (ret == -1) {
			perror("Poll failed");
		}

		if (ret > 0) {
			_processPollfds();
		}

		_checkAllConnectionStatus();
		_updatePollfds();
    }
}

void ServerManager::_processPollfds() {
	for (auto& pollfd : _pollfds) {
		auto& server = _serverMap.at(pollfd.fd).get();

		if (pollfd.revents & POLLHUP) {
			server.closeConnection(pollfd.fd);
			continue;
		}

		if (pollfd.revents & POLLIN) {
			if (server.getServerFd() == pollfd.fd) {
				int clientFd = server.addConnection();

				if (clientFd >= 0) {
					_serverMap.emplace(clientFd, server);
					_newPollfds.insert(clientFd);
				}

				continue;
			}

			server.process(pollfd.fd, pollfd.events);
		}

		if (pollfd.revents & POLLOUT) {
			server.sendResponse(pollfd.fd, pollfd.events);
		}
	}

}

void ServerManager::_checkAllConnectionStatus() {
	for (auto& server: _servers) {
		std::erase_if(server.getAllConnections(), [this](auto& pair) {
			if (pair.second.isClosed()) {
				_stalePollfds.insert(pair.first);
				return true;
			}

			// if (pair.second.isTimedOut()) {
			// 	pair.second.close();
			// 	_stalePollfds.insert(pair.first);
			// 	return true;
			// }

			return false;
		});
		// for (auto& [fd, connection] : server.getAllConnections()) {
		// 	if (connection.isClosed()) {
		// 		_stalePollfds.insert(fd);
		// 		server.removeConnection(fd);
		// 	}
		// }
	}
}

void ServerManager::_updatePollfds() {
	std::erase_if(_pollfds, [this](const struct ::pollfd& pollfd) {
		return (_stalePollfds.contains(pollfd.fd));
	});

	_pollfds.reserve(_pollfds.size() + _newPollfds.size());

	for (const int fd : _newPollfds) {
		_pollfds.push_back({ fd, POLLIN, 0 });
	}

	_stalePollfds.clear();
	_newPollfds.clear();
}
