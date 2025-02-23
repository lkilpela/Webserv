#pragma once

#include <sys/types.h>
#include <array>

class WorkerProcess {
	public:
		enum Status : short {
			RUNNING,
			PENDING_TERMINATION,
			TERMINATED
		};

		WorkerProcess() = delete;
		WorkerProcess(int clientFd, std::array<int, 2> pipeFds);
		~WorkerProcess() = default;

		void updateStatus();
		void terminate();

		Status getStatus() const;
		int getClientFd() const;
		std::array<int, 2> getPipeFds() const;
		pid_t getPid() const;

	private:
		std::array<int, 2> _pipeFds;
		int _clientFd;
		Status _status = { RUNNING };
		pid_t _pid;
};