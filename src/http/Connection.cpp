#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <algorithm>
#include <array>
#include "http/Connection.hpp"
#include "http/utils.hpp"
#include "utils/common.hpp"

namespace http {
	Connection::Connection(int clientSocket, const ServerConfig& serverConfig)
		: _clientSocket(clientSocket)
		, _serverConfig(serverConfig) {
	}

	void Connection::append(char *data, ssize_t size) {
		if (isClosed()) {
			return;
		}

		if (_isTimedOut()) {
			this->close();
			return;
		}

		_buffer.reserve(_buffer.size() + size);
		_buffer.insert(_buffer.end(), data, data + size);
		_lastReceived = std::chrono::steady_clock::now();
		_processBuffer();

		if (_request.getStatus() == Request::Status::BAD || _request.getStatus() == Request::Status::COMPLETE) {
			_queue.emplace(std::move(_request), Response(_clientSocket));
			_request.clear();
		}
	}

	void Connection::sendResponse() {
		if (isClosed() || _queue.empty()) {
			return;
		}

		auto& [req, res] = _queue.front();

		if (res.getStatus() != Response::Status::READY) {
			return;
		}

		if (res.send()) {
			const auto responseStatusCode = res.getStatusCode();
			auto connectionHeader = req.getHeader(Header::CONNECTION);
			_queue.pop();

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

	bool Connection::isClosed() const {
		return (_clientSocket == -1);
	}

	Request* Connection::getRequest() {
		if (_queue.size() == 0) {
			return nullptr;
		}

		auto& pair = _queue.front();
		return &pair.first;
	}

	Response* Connection::getResponse() {
		if (_queue.size() == 0) {
			return nullptr;
		}

		auto& pair = _queue.front();
		return &pair.second;
	}

	bool Connection::_isTimedOut() const {
		auto elapsedTime = std::chrono::steady_clock::now() - _lastReceived;

		if (elapsedTime > std::chrono::milliseconds(_serverConfig.timeoutIdle)) {
			return true;
		}

		return false;
	}

	void Connection::_processBuffer() {
		if (_request.getStatus() == Request::Status::INCOMPLETE) {
			_parseHeader();
		}

		if (_request.getStatus() == Request::Status::HEADER_COMPLETE) {
			if (_request.isChunked()) {
				_parseChunkedBody();
			} else {
				_parseBody();
			}
		}
	}

	void Connection::_parseHeader() {
		auto begin = _buffer.begin();
		auto end = _buffer.end();
		auto it = utils::findDelimiter(begin, end, {'\r', '\n', '\r', '\n'});

		if (it == end && _buffer.size() > MAX_REQUEST_HEADER_SIZE) {
			_request.setStatus(Request::Status::BAD);
			return;
		}

		if (it != end) {
			try {
				_request = std::move(Request::parseHeader(std::string(begin, it + 4)));
				_buffer.erase(begin, it + 4);
			} catch (const std::invalid_argument &e) {
				_request.setStatus(Request::Status::BAD);
			}
		}
	}

	void Connection::_parseBody() {
		std::size_t contentLength = _request.getContentLength();

		if (contentLength == 0) {
			_request.setStatus(Request::Status::COMPLETE);
			return;
		}

		if (_buffer.size() >= contentLength) {
			_request.appendBody(_buffer.begin(), _buffer.begin() + contentLength);
			_buffer.erase(_buffer.begin(), _buffer.begin() + contentLength + 4);
			_request.setStatus(Request::Status::COMPLETE);
		}
	}

	void Connection::_parseChunkedBody() {
		std::vector<uint8_t> buffer;
		bool isChunkEnd = false;
		std::size_t currentPos = 0;
		auto begin = _buffer.begin();
		auto end = _buffer.end();

		buffer.reserve(_buffer.size());

		while (true) {
			auto start = begin + currentPos;
			auto firstIt = utils::findDelimiter(start, end, {'\r', '\n'});
			auto secondIt = (firstIt == end) ? end : utils::findDelimiter(firstIt + 2, end, {'\r', '\n'});;

			if (firstIt == end || secondIt == end) {
				return;
			}

			try {
				std::size_t chunkSize = parseChunkSize(std::string(start, firstIt));

				if (chunkSize == 0) {
					isChunkEnd = true;
					currentPos += secondIt + 2 - start;
					break;
				}

				if (static_cast<std::size_t>(std::distance(firstIt + 2, secondIt)) != chunkSize) {
					throw std::invalid_argument("Error: chunk size does not match the actual data size.");
				}

				buffer.insert(buffer.end(), firstIt + 2, secondIt);
				currentPos += secondIt + 2 - start;
			} catch (const std::invalid_argument& e) {
				_request.setStatus(Request::Status::BAD);
				return;
			}
		}

		_request.appendBody(buffer.begin(), buffer.end());
		_buffer.erase(begin, begin + currentPos);

		if (isChunkEnd) {
			_request.setStatus(Request::Status::COMPLETE);
		}
	}
}
