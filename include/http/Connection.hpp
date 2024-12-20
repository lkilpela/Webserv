#pragma once

#include <vector>
#include <queue>
#include <utility>
#include <chrono>
#include "Request.hpp"
#include "Response.hpp"

namespace http {
	class Connection {
		public:
			Connection(int clientSocket, int msTimeout);
			Connection(const Connection&) = delete;

			Connection& operator=(const Connection&) = delete;

			ssize_t readData(int clientSocket);
			const std::pair<Request, Response>& getRequestResponse();
			void close();

			bool isTimedOut() const;

		private:
			int _clientSocket;
			int _msTimeout;
			std::vector<std::uint8_t> _buffer;
			std::queue<std::pair<Request, Response>> _queue;
			std::chrono::steady_clock::time_point _lastReceived;
			bool _isClosed { false };

			void _processBuffer();
	};
}
