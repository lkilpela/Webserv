#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <algorithm>
#include <array>
#include "http/Connection.hpp"
#include "utils/common.hpp"
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

		if (_request.getStatus() == Request::Status::BAD || _request.getStatus() == Request::Status::COMPLETE) {
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
			const auto responseStatusCode = res.getStatusCode();
			auto connectionHeader = req.getHeader(Header::CONNECTION);
			_processedQueue.pop();

			if (
				(connectionHeader.has_value() && *connectionHeader == "close")
				|| responseStatusCode == StatusCode::BAD_REQUEST_400
				|| responseStatusCode == StatusCode::INTERNAL_SERVER_ERROR_500
			) {
				this->close();
				return;
			}
		}
	}

	void Connection::close() {
		if (_clientSocket == -1) {
			return;
		}

		if (::close(_clientSocket) < 0) {
			std::cerr << "Failed to close socket " << _clientSocket << ": " << strerror(errno) << std::endl;
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

	void Connection::_processBuffer() {
		if (_request.getStatus() == Request::Status::INCOMPLETE) {
			_handleHeader();
		}

		if (_request.getStatus() == Request::Status::HEADER_COMPLETE) {
			if (_request.isChunked()) {
				_handleChunkedBody();
			} else {
				_handleBody();
			}
		}
	}

	void Connection::_handleHeader() {
		auto begin = _requestBuffer.begin();
		auto end = _requestBuffer.end();
		auto it = utils::findDelimiter(begin, end, {'\r', '\n', '\r', '\n'});

		if (it == end && _requestBuffer.size() > MAX_REQUEST_HEADER_SIZE) {
			_request.setStatus(Request::Status::BAD);
			return;
		}

		if (it != end) {
			std::string rawRequestHeader(begin, it + 4);
			_requestBuffer.erase(begin, it + 4);

			try {
				_request = std::move(Request::parseHeader(rawRequestHeader));
			} catch (const std::invalid_argument &e) {
				_request.setStatus(Request::Status::BAD);
			}
		}
	}

	void Connection::_handleChunkedBody() {
		std::vector<uint8_t> buffer;
		bool isChunkEnd = false;
		std::size_t currentPos = 0;
		auto begin = _requestBuffer.begin();
		auto end = _requestBuffer.end();

		buffer.reserve(_requestBuffer.size());

		while (true) {
			auto start = begin + currentPos;
			auto firstIt = utils::findDelimiter(start, end, {'\r', '\n'});
			auto secondIt = (firstIt == end) ? end : utils::findDelimiter(firstIt + 2, end, {'\r', '\n'});;

			if (firstIt == end || secondIt == end) {
				return;
			}

			std::size_t chunkSize = parseChunkSize(std::string(start, firstIt));

			if (chunkSize == 0) {
				isChunkEnd = true;
				currentPos += secondIt + 2 - start;
				break;
			}

			if (std::distance(firstIt + 2, secondIt) != chunkSize) {
				_request.setStatus(Request::Status::BAD);
				return;
			}

			buffer.insert(buffer.end(), firstIt + 2, secondIt);
			currentPos += secondIt + 2 - start;
		}

		_request.appendBody(buffer.begin(), buffer.end());
		_requestBuffer.erase(begin, begin + currentPos);

		if (isChunkEnd) {
			_request.setStatus(Request::Status::COMPLETE);
		}
	}

	void Connection::_handleBody() {
		std::size_t bodySize = _request.getBodySize();

		if (bodySize == 0) {
			_request.setStatus(Request::Status::COMPLETE);
			return;
		}

		if (_requestBuffer.size() >= bodySize) {
			_request.appendBody(_requestBuffer.begin(), _requestBuffer.begin() + bodySize);
			_requestBuffer.erase(_requestBuffer.begin(), _requestBuffer.begin() + bodySize);
			_request.setStatus(Request::Status::COMPLETE);
		}
	}
}
