#pragma once

#include <vector>
#include <queue>
#include <utility>
#include <chrono>
#include "Request.hpp"
#include "Response.hpp"

namespace http {
	class Connection {
		private:
			int _msTimeout;
			std::vector<std::uint8_t> _buffer;
			std::queue<std::pair<Request, Response>> _queue;
			std::chrono::steady_clock::time_point _lastReceived;

		public:
			explicit Connection(int msTimeout);
			Connection(const Connection&) = delete;

			Connection& operator=(const Connection&) = delete;

			ssize_t readData(int clientSocket);
			const std::pair<Request, Response>& getRequestResponse();

			bool isTimedOut() const;
	};
}