#include <sys/wait.h>
#include <errno.h>
#include "WorkerProcess.hpp"

WorkerProcess::WorkerProcess(int clientFd, std::array<int, 2> pipeFds) : _clientFd(clientFd), _pipeFds(pipeFds) {}

void WorkerProcess::updateStatus() {
	pid_t pid = ::waitpid(_pid, NULL, WNOHANG);

	if (pid > 0 || (pid == -1 && errno == ECHILD)) {
		_status = TERMINATED;
	}
}

void WorkerProcess::terminate() {
	// if (_status == TERMINATED) {
	// 	return;
	// }

	::kill(_pid, SIGTERM);
	::close(_pipeFds[0]);
	_pipeFds[0] == -1;
	_status = PENDING_TERMINATION;
	pid_t pid = ::waitpid(_pid, NULL, WNOHANG);

	if (pid > 0 || (pid == -1 && errno == ECHILD)) {
		_status = TERMINATED;
	}
}

WorkerProcess::Status WorkerProcess::getStatus() const {
	return _status;
}

int WorkerProcess::getClientFd() const {
	return _clientFd;
}

std::array<int, 2> WorkerProcess::getPipeFds() const {
	return _pipeFds;
}

pid_t WorkerProcess::getPid() const {
	return _pid;
}
