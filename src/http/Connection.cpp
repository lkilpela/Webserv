#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <array>
#include "http/Connection.hpp"
#include "http/utils.hpp"

namespace http {

	Connection::Connection(int clientSocket, int msTimeout, std::function<void (Request&, Response&)> process)
		: _clientSocket(clientSocket)
		, _msTimeout(msTimeout)
		, _process(process)
		, _lastReceived(std::chrono::steady_clock::now()) {
	}

	void Connection::readRequest(std::uint8_t *buffer, size_t size) {
		_requestBuffer.reserve(_requestBuffer.size() + size);
		_requestBuffer.insert(_requestBuffer.end(), buffer, buffer + size);
		_lastReceived = std::chrono::steady_clock::now();
		_processBuffer();

		if (_request.getStatus() != Request::Status::INCOMPLETE) {
			Response response(_clientSocket);

			_process(_request, response);
			_processedQueue.emplace(std::move(_request), std::move(response));
			_request.clear();
		}
	}

	void Connection::sendResponse() {
		if (_processedQueue.empty()) {
			return;
		}

		auto& [req, res] = _processedQueue.front();

		if (res.send()) {
			const auto code = res.getStatusCode();
			auto header = req.getHeader(Header::CONNECTION);

			if (
				(header.has_value() && *header == "close")
				|| code == StatusCode::BAD_REQUEST_400
				|| code == StatusCode::INTERNAL_SERVER_ERROR_500
			) {
				this->close();
				return;
			}

			_processedQueue.pop();
		}
	}

	void Connection::close() {
		if (_clientSocket == -1) {
			return;
		}

		if (::close(_clientSocket) < 0) {
			std::cerr
				<< "Failed to close socket " << _clientSocket << ": "
				<< strerror(errno) << std::endl;
		}

		_clientSocket = -1;
	}

	bool Connection::isTimedOut() const {
		auto elapsedTime = std::chrono::steady_clock::now() - _lastReceived;
		return (elapsedTime > std::chrono::milliseconds(_msTimeout));
	}

	bool Connection::isClosed() const {
		return (_clientSocket == -1);
	}

	// Unfinished
	void Connection::_processBuffer() {
		using enum Request::Status;

		std::size_t pos = findBlankLine(_requestBuffer.begin(), _requestBuffer.end());

		if (pos == std::string::npos && _requestBuffer.size() > MAX_REQUEST_HEADER_SIZE) {
			_request.setStatus(BAD_REQUEST);
			return;
		}

		if (pos != std::string::npos) {
			// parse here

		}
	}
}
