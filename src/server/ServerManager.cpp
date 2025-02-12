#include <sys/socket.h>
#include "ServerManager.hpp"
#include "utils/index.hpp"

ServerManager::ServerManager(const Config& config) : _config(config) {
	_servers.reserve(_config.servers.size());

	for (std::size_t i = 0; i < _config.servers.size(); i++) {
		const ServerConfig& serverConfig = _config.servers[i];
		_servers.push_back(Server(serverConfig));
    }

	for (auto& server : _servers) {
		for (const int serverFd : server.getFds()) {
			_serverMap.emplace(serverFd, std::ref(server));
			_pollfds.push_back({ serverFd, POLLIN, 0 });
		}
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

		_pruneClosedConnections();
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
			if (server.getFds().contains(pollfd.fd)) {
				int clientFd = server.addConnection(pollfd.fd);

				if (clientFd >= 0) {
					_serverMap.emplace(clientFd, std::ref(server));
					_newFds.insert(clientFd);
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

void ServerManager::_pruneClosedConnections() {
	for (auto& server: _servers) {
		auto& connectionMap = server.getConnectionMap();

		for (auto it = connectionMap.begin(); it != connectionMap.end();) {
			auto& [fd, connection] = *it;

			// if (connection.isTimedOut()) {
			// 	connection.close();
			// }

			if (connection.isClosed()) {
				_serverMap.erase(fd);
				_staleFds.insert(fd);
				it = connectionMap.erase(it);
				continue;
			}

			it++;
		}
	}
}

void ServerManager::_updatePollfds() {
	_pollfds.reserve(_pollfds.size() + _newFds.size());

	for (const int fd : _newFds) {
		_pollfds.push_back({ fd, POLLIN, 0 });
	}

	std::erase_if(_pollfds, [this](const struct ::pollfd& pollfd) {
		return (_staleFds.contains(pollfd.fd));
	});

	_staleFds.clear();
	_newFds.clear();
}
