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

	void Connection::read() {
		if (isClosed()) {
			return;
		}

		unsigned char buf[4096];
		ssize_t bytesRead = recv(_clientSocket, buf, sizeof(buf), MSG_NOSIGNAL);

		if (bytesRead == 0) {
			this->close();
			return;
		}

		if (bytesRead > 0) {
			_buffer.reserve(_buffer.size() + bytesRead);
			_buffer.insert(_buffer.end(), buf, buf + bytesRead);
			// std::cout << "_buffer's content=" << std::string(_buffer.begin(), _buffer.end()) <<std::endl;
			_lastReceived = std::chrono::steady_clock::now();
			_processBuffer();

			if (_request.getStatus() == Request::Status::BAD || _request.getStatus() == Request::Status::COMPLETE) {
				_queue.emplace(std::move(_request), Response(_clientSocket));
				_request.clear();
			}
		}
	}

	bool Connection::sendResponse() {
		if (isClosed() || _queue.empty()) {
			return false;
		}

		auto& [req, res] = _queue.front();

		if (res.getStatus() != Response::Status::READY) {
			return false;
		}

		if (res.send()) {
			const StatusCode code = res.getStatusCode();
			auto connectionHeader = req.getHeader(Header::CONNECTION);
			_queue.pop();

			if (
				connectionHeader.value_or("") == "close"
				|| code == StatusCode::BAD_REQUEST_400
				|| code == StatusCode::REQUEST_TIMEOUT_408
				|| code == StatusCode::INTERNAL_SERVER_ERROR_500
				|| code == StatusCode::SERVICE_UNAVAILABLE_503
				|| code == StatusCode::GATEWAY_TIMEOUT_504
			) {
				this->close();
			}

			return true;
		}

		return false;
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

	bool Connection::isTimedOut() const {
		// auto elapsedTime = std::chrono::steady_clock::now() - _lastReceived;

		// if (elapsedTime > std::chrono::milliseconds(_serverConfig.timeoutIdle)) {
		// 	return true;
		// }

		return false;
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

	void Connection::_processBuffer() {
		if (_request.getStatus() == Request::Status::INCOMPLETE) {
			_parseHeader();
		}
		std::cout << "_request.isChunked()? " << _request.isChunked() << std::endl;
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
			std::cout << "_parseHeader error > MAX_REQUEST_HEADER_SIZE, _buffer.size()=" << _buffer.size() << std::endl;
			_request.setStatus(Request::Status::BAD);
			return;
		}

		if (it != end) {
			try {
				_request = std::move(Request::parseHeader(std::string(begin, it + 4)));
				_buffer.erase(begin, it + 4);
			} catch (const std::invalid_argument &e) {
				std::cout << "_parseHeader error @Request::parseHeader()" << std::endl;
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
		auto begin = _buffer.begin();
		auto end = _buffer.end();
		auto currentPos = begin;

		// std::cout << "processing=" << std::string(begin, end) << std::endl;
		buffer.reserve(_buffer.size());

		while (true) {
			auto delimPos = utils::findDelimiter(currentPos, end, {'\r', '\n'});

			if (delimPos == end || std::distance(delimPos, end) < 2) {
				return;
			}

			// if (delimPos == currentPos) {
			// 	std::cout << "delim == currentPos" << *delimPos << *currentPos << std::endl;
			// 	return;
			// }

			try {
				std::size_t chunkSize = parseChunkSize(std::string(currentPos, delimPos));
				std::size_t distance = static_cast<std::size_t>(std::distance(delimPos + 2, end));

				if (distance < chunkSize) {
					break;
				}

				if (chunkSize == 0) {
					if (distance < 2) {
						break;
					}

					if (*(delimPos + 2) == '\r' && *(delimPos + 3) == '\n') {
						isChunkEnd = true;
						currentPos = delimPos + 4;
						break;
					}

					throw std::invalid_argument(R"(Error: Chunk body did not end with 0\r\n\r\n)");
				}

				currentPos = delimPos + 2;
				buffer.insert(buffer.end(), currentPos, currentPos + chunkSize);
				currentPos += chunkSize;
			} catch (const std::invalid_argument& e) {
				std::cout << "_parseHeader error invalid_argument" << std::endl;
				_request.setStatus(Request::Status::BAD);
				return;
			}
		}

		_request.appendBody(buffer.begin(), buffer.end());
		_buffer.erase(begin, currentPos);

		if (isChunkEnd) {
			_request.setStatus(Request::Status::COMPLETE);
		}
	}
}
