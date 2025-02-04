#include <sys/socket.h>
#include "ServerManager.hpp"
#include "utils/index.hpp"

using PollIterator = std::vector<pollfd>::iterator;

ServerManager::ServerManager(const Config& config) : _config(config) {
	for (int i = 0; i < config.servers.size(); i++) {
		const int port = config.ports[i];
		const ServerConfig serverConfig = config.servers[i];

		int serverFd = utils::createPassiveSocket(port, 128, true);
		_servers.push_back(Server(serverFd, serverConfig));
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

		_checkTimeout();
		_updatePollfds();
    }
}

void ServerManager::_processPollfds() {
	for (auto& pollfd : _pollfds) {
		if (pollfd.revents & POLLHUP) {
			_findServer(pollfd.fd).close
			_stalePollfds.insert(pollfd.fd);
			continue;
		}

		if (pollfd.revents & POLLIN) {
			auto it = std::ranges::find_if(_servers, [fd = pollfd.fd](const Server& server) {
				return server.getServerFd() == fd;
			});

			if (it != _servers.end()) {
				int clientFd = it->addConnection();

				if (clientFd >= 0) {
					_newPollfds.insert(clientFd);
				}

				continue;
			}
			// for (auto server : _servers) {
			// 	if (server.getServerFd() == pollfd.fd) {

			// 	}

			// 	server.process(pollfd.fd);
			// }
		}

		if (pollfd.revents & POLLOUT) {
			for (auto server : _servers) {
				server.sendResponse(pollfd.fd);
			}
		}
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
