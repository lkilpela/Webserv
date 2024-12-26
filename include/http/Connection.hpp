#pragma once

#include <vector>
#include <queue>
#include <chrono>
#include "Request.hpp"
#include "Response.hpp"

namespace http {
	class Connection {
		public:
			Connection(int clientSocket, int msTimeout);
			Connection(const Connection&) = delete;
			Connection& operator=(const Connection&) = delete;

			void readRequest(std::uint8_t *buffer, size_t size);
			void sendResponse();
			void close();

			bool isTimedOut() const;

		private:
			int _clientSocket;
			int _msTimeout;
			std::vector<std::uint8_t> _requestBuffer;
			std::queue<Response> _queue;
			std::chrono::steady_clock::time_point _lastReceived;

			void _processBuffer(std::size_t pos);
	};
}
