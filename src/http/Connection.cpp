#include "http/Connection.hpp"

namespace http {

	Connection::Connection(struct pollfd& pollFd, int timeout_ms)
		: _pollFd(pollFd)
		, _timeout_ms(timeout_ms)
		, _lastReceived(std::chrono::steady_clock::now()) {
	}

	const bool Connection::isTimeout() const {
		const auto elapsedTime = std::chrono::steady_clock::now() - _lastReceived;
		return (elapsedTime > std::chrono::milliseconds(_timeout_ms));
	}

	void Connection::close() {

	}

}