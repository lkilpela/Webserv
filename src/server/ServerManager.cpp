#include <sys/socket.h>
#include <sys/wait.h>
#include "ServerManager.hpp"
#include "utils/index.hpp"
#include "SignalHandle.hpp"

ServerManager::ServerManager(const Config& config) : _config(config) {
	_servers.reserve(_config.servers.size());

	for (std::size_t i = 0; i < _config.servers.size(); i++) {
		const ServerConfig& serverConfig = _config.servers[i];
		_servers.push_back(Server(serverConfig));
		_servers.back().addCgiHandler();
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
	_serverByFd.emplace(fd, server);
	std::cout << "Added fd " << fd << " to pollfds" << std::endl;
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
	std::cout << "Removed fd " << fd << " to pollfds" << std::endl;
}

void ServerManager::_processPollFds() {
	for (auto& pollFd : _pollFds) {
		auto& server = _serverByFd.at(pollFd.fd).get();
		const bool isServerFd = server.getServerFds().contains(pollFd.fd);

		if ((pollFd.revents & POLLHUP) && !isServerFd) {
			// server.close(pollFd.fd);
			std::cout << "POLLHUP occurred to fd " << pollFd.fd << std::endl;
			continue;
		}

		if (pollFd.revents & POLLIN) {
			if (isServerFd) {
				server.addConnection(pollFd.fd);
				continue;
			}
			std::cout << "POLLIN occurred to fd " << pollFd.fd << std::endl;
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
	auto& map = server.getManagedConnections();

	for (auto it = map.begin(); it != map.end();) {
		auto& [fd, connection] = *it;

		if (!connection.isClosed() && !_pollFdIndexByFd.contains(fd)) {
			_addPollFd(fd, server);
			it++;
			continue;
		}

		if (!connection.isClosed() && connection.isTimedOut()) {
			server.closeConnection(connection);
		}

		if (connection.isClosed()) {
			_removePollFd(fd);
			it = map.erase(it);
			continue;
		}

		it++;
	}
}

void ServerManager::_updatePipeConnections(Server& server) {
	auto& map = server.getWorkerProcesses();

	using enum WorkerProcess::Status;

	for (auto it = map.begin(); it != map.end();) {
		auto& [fd, process] = *it;
		const bool isInPollFds = _pollFdIndexByFd.contains(fd);

		if (process.status == RUNNING && !isInPollFds) {
			_addPollFd(fd, server);
			it++;
			continue;
		}

		if (process.status != RUNNING && process.pipeFds[0] == -1 && isInPollFds) {
			_removePollFd(fd);
		}

		if (process.status == PENDING_TERMINATION) {
			pid_t pid = ::waitpid(process.pid, NULL, WNOHANG);

			if (pid > 0 || (pid == -1 && errno == ECHILD)) {
				process.status = TERMINATED;
			}
		}

		if (process.status == TERMINATED) {
			it = map.erase(it);
			continue;
		}

		it++;
	}
}

void ServerManager::_shutDownServers(){
	for (auto& server : _servers)
		server.shutdown();
}
