#include <sys/socket.h>
#include <array>
#include "http/Connection.hpp"

namespace http {

	Connection::Connection(int msTimeout) : _msTimeout(msTimeout) {}

	ssize_t Connection::readData(int clientSocket) {
		std::array<std::uint8_t, 1024> buf;
		ssize_t bytesRead = recv(clientSocket, buf.data(), buf.size(), 0);
		if (bytesRead > 0) {
			_buffer.insert(_buffer.end(), buf.begin(), buf.begin() + bytesRead);
			_lastReceived = std::chrono::steady_clock::now();
			Request req = Request::parse(_buffer.begin(), _buffer.end());
			const auto& status = req.getStatus();
			if (status == RequestStatus::COMPLETE || status == RequestStatus::BAD_REQUEST) {
				_queue.emplace(std::move(req), Response {});
				// update _buffer
			}
		}
		return bytesRead;
	}

	const std::pair<Request, Response>& Connection::getRequestResponse() {
		return _queue.front();
	}

	bool Connection::isTimedOut() const {
		auto elapsedTime = std::chrono::steady_clock::now() - _lastReceived;
		return (elapsedTime > std::chrono::milliseconds(_msTimeout));
	}

}