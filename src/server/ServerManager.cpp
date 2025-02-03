#include <sys/socket.h>
#include "ServerManager.hpp"
#include "utils/index.hpp"

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
		// if (sigintReceived){
		// 	_cleanup();
		// 	break;
		// }

        if (::poll(_pollfds.data(), _pollfds.size(), 50) == -1)
			perror("Poll failed");

        for (auto it = _pollfds.begin(); it != _pollfds.end();) {
			const auto [fd, events, revents] = *it; // fd here can be server, client or pipe file descriptor

			for(auto& server: _servers) {
				if (server.getServerFd() == fd) {
					if (revents & POLLIN) {
						int clientFd = server.addConnection();
						if (clientFd >= 0) {
							struct ::pollfd clientPollFd { clientFd, POLLIN, 0 };
							_pollfds.push_back(clientPollFd);
						}
					}
				} else {
					server.process(fd);
					// auto& connection = server.findConnection(fd);
					// // auto& connection = _connectionByFd[fd];

					// if (revents & POLLHUP) {
					// 	connection.close();
					// } else {
					// 	_read(*it, connection);
					// 	_process(*it, connection);
					// 	_sendResponse(*it, connection);
					// }

					// if (connection.isClosed()) {
					// 	_connectionByFd.erase(it->fd);
					// 	it = _pollfds.erase(it);
					// 	continue;
					// }
				}

			}
			// if fd is not serverFD then we execute code below

			it++;
		}
    }
}