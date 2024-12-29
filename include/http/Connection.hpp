#pragma once

#include <vector>
#include <queue>
#include <utility>
#include <chrono>
#include <functional>
#include "Request.hpp"
#include "Response.hpp"

namespace http {
	class Connection {
		public:
			Connection(int clientSocket, int msTimeout, std::function<void (Request&, Response&)> process);
			Connection(const Connection&) = delete;
			Connection& operator=(const Connection&) = delete;

			void readRequest(std::uint8_t *buffer, size_t size);
			void sendResponse();
			void close();

			bool isTimedOut() const;
			bool isClosed() const;

		private:
			int _clientSocket;
			int _msTimeout;
			std::function<void (Request&, Response&)> _process;
			std::vector<std::uint8_t> _requestBuffer;
			std::queue<std::pair<Request, Response>> _processedQueue;
			Request _request { Request::Status::INCOMPLETE };
			std::chrono::steady_clock::time_point _lastReceived;

			void _processBuffer();
			void _handleHeader();
			void _handleChunkedBody();
			void _handleBody();
	};
}
