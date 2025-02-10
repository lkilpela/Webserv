#pragma once

#include <vector>
#include <queue>
#include <utility>
#include <chrono>
#include <functional>
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"

namespace http {
	class Connection {
		public:
			Connection(int clientSocket, const ServerConfig& serverConfig);
			Connection(const Connection&) = default;
			~Connection() = default;

			Connection& operator=(const Connection&) = default;

			void append(const std::uint8_t* data, size_t size);
			bool sendResponse();
			void close();

			bool isClosed() const;
			bool isTimedOut() const;

			Request* getRequest();
			Response* getResponse();

		private:
			int _clientSocket;
			const ServerConfig& _serverConfig;
			Request _request { Request::Status::INCOMPLETE };
			std::vector<std::uint8_t> _buffer;
			std::queue<std::pair<Request, Response>> _queue;
			std::chrono::steady_clock::time_point _lastReceived;

			void _processBuffer();
			void _parseHeader();
			void _parseBody();
			void _parseChunkedBody();
	};
}
