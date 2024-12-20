#include <sys/socket.h>
#include <unistd.h>
#include <array>
#include "http/Connection.hpp"

namespace http {

	Connection::Connection(int clientSocket, int msTimeout)
		: _clientSocket(clientSocket)
		, _msTimeout(msTimeout) {
	}

	ssize_t Connection::readData(int clientSocket) {
		std::array<std::uint8_t, 1024> buf;
		ssize_t bytesRead = recv(clientSocket, buf.data(), buf.size(), 0);
		if (bytesRead > 0) {
			_buffer.insert(_buffer.end(), buf.begin(), buf.begin() + bytesRead);
			_lastReceived = std::chrono::steady_clock::now();
			_processBuffer();
		}
		if (bytesRead == 0)
		return bytesRead;
	}

	const std::pair<Request, Response>& Connection::getRequestResponse() {
		return _queue.front();
	}

	void Connection::close() {
		::close(_clientSocket);
		_clientSocket = -1;
	}

	bool Connection::isTimedOut() const {
		auto elapsedTime = std::chrono::steady_clock::now() - _lastReceived;
		return (elapsedTime > std::chrono::milliseconds(_msTimeout));
	}

	void Connection::_processBuffer() {
		Request req = Request::parse(_buffer.begin(), _buffer.end());
			const auto& status = req.getStatus();
			if (status == Request::Status::COMPLETE || status == Request::Status::BAD_REQUEST) {
				_queue.emplace(std::move(req), Response { _clientSocket });
				// update _buffer
			}
	}
}
