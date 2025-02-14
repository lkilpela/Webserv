#include <sys/socket.h>
#include "ServerManager.hpp"
#include "utils/index.hpp"
#include "SignalHandle.hpp"

ServerManager::ServerManager(const Config& config) : _config(config) {
	_servers.reserve(config.servers.size());

	for (std::size_t i = 0; i < config.servers.size(); i++) {
		const ServerConfig serverConfig = config.servers[i];
		_servers.push_back(Server(serverConfig));
    }

	for (auto& server : _servers) {
		for (const int serverFd : server.getServerFds()) {
			_addPollFd(serverFd, server);
		}
	}
}

void ServerManager::listen() {
	while (_pollFds.size()) {
		if (sigIntReceived){
			_shutDownServers();
			break ;
		}
        int ret = ::poll(_pollFds.data(), _pollFds.size(), 100);

		if (ret == -1) {
			if (errno == EINTR)
            	continue;
			else
				perror("Poll failed");
		}

		if (ret > 0) {
			_processPollFds();
		}

		_updatePollFds();
    }
}

void ServerManager::_addPollFd(int fd, Server& server) {
	auto it = _pollFdIndexByFd.find(fd);

	if (it != _pollFdIndexByFd.end()) {
		return;
	}

	_pollFds.push_back({ fd, POLLIN, 0 });
	_pollFdIndexByFd[fd] = _pollFds.size() - 1;
	_serverByFd.emplace(fd, std::ref(server));
}

void ServerManager::_removePollFd(int fd) {
	auto it = _pollFdIndexByFd.find(fd);

	if (it == _pollFdIndexByFd.end()) {
		return;
	}

	const std::size_t index = it->second;

	_pollFdIndexByFd.erase(it);
	_serverByFd.erase(fd);

	if (index != _pollFds.size() - 1) {
		std::swap(_pollFds[index], _pollFds.back());
		_pollFdIndexByFd[_pollFds[index].fd] = index;
	}

	_pollFds.pop_back();
}

void ServerManager::_processPollFds() {
	for (auto& pollFd : _pollFds) {
		auto& server = _serverByFd.at(pollFd.fd).get();
		const bool isServerFd = server.getServerFds().contains(pollFd.fd);

		if ((pollFd.revents & POLLHUP) && !isServerFd) {
			server.close(pollFd.fd);
			continue;
		}

		if (pollFd.revents & POLLIN) {
			if (isServerFd) {
				server.addClientTo(pollFd.fd);
				continue;
			}

			server.process(pollFd.fd, pollFd.events);
		}

		if (pollFd.revents & POLLOUT) {
			server.sendResponse(pollFd.fd, pollFd.events);
		}
	}
}

void ServerManager::_updatePollFds() {
	for (auto& server: _servers) {
		_updateClientConnections(server);
		_updatePipeConnections(server);
	}
}

void ServerManager::_updateClientConnections(Server& server) {
	auto& clients = server.getClients();

	for (auto it = clients.begin(); it != clients.end();) {
		auto& [fd, connection] = *it;

		if (connection.isTimedOut()) {
			connection.close();
		}

		if (connection.isClosed()) {
			_removePollFd(fd);
			it = clients.erase(it);
			continue;
		}

		_addPollFd(fd, server);
		it++;
	}
}

void ServerManager::_updatePipeConnections(Server& server) {
	auto& pipeConnections = server.getPipeProcess();

	for (auto it = pipeConnections.begin(); it != pipeConnections.end();) {
		auto& [pipeFd, process] = *it;

		if (process.isPipeClosed) {
			_removePollFd(pipeFd);
			it = pipeConnections.erase(it);
			continue;
		}

		_addPollFd(pipeFd, server);
		it++;
	}
}

void ServerManager::_shutDownServers(){
	for (auto& server : _servers)
		server._shutDownServer();
}