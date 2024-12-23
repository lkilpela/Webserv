#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <array>
#include "http/Connection.hpp"
#include "http/utils.hpp"

namespace http {

	Connection::Connection(int clientSocket, int msTimeout)
		: _clientSocket(clientSocket)
		, _msTimeout(msTimeout)
		, _lastReceived(std::chrono::steady_clock::now()) {
	}

	void Connection::readRequest(std::uint8_t *buffer, size_t size) {
		_requestBuffer.reserve(_requestBuffer.size() + size);
		_requestBuffer.insert(_requestBuffer.end(), buffer, buffer + size);
		_lastReceived = std::chrono::steady_clock::now();

		std::size_t pos = findBlankLine(_requestBuffer.begin(), _requestBuffer.end());

		if (pos == std::string::npos && _requestBuffer.size() > MAX_REQUEST_HEADER_SIZE) {
			_responseQueue.emplace(Request { Request::Status::BAD_REQUEST }, Response { _clientSocket });
			return;
		}
			_processBuffer(pos);

	}

	void Connection::sendResponse() {
		if (_responseQueue.empty())
			return;

		auto& response = _responseQueue.front();

		if (response.send())
			_responseQueue.pop();
	}

	bool Connection::isTimedOut() const {
		auto elapsedTime = std::chrono::steady_clock::now() - _lastReceived;
		return (elapsedTime > std::chrono::milliseconds(_msTimeout));
	}

	void Connection::close() {
		if (_clientSocket == -1)
			return;

		if (::close(_clientSocket) < 0)
			std::cerr << "Failed to close socket " << _clientSocket << ": " << strerror(errno) << std::endl;
			
		_clientSocket = -1;
	}

	void Connection::_processBuffer(std::size_t pos) {
		using enum Request::Status;

		Request req = Request::parse(_requestBuffer.begin(), _requestBuffer.end());

		if (req.getStatus() == COMPLETE || req.getStatus() == BAD_REQUEST) {
			_responseQueue.emplace(std::move(req), Response { _clientSocket });
			// update _requestBuffer
		}
	}
}
