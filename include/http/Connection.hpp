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
			Connection(
				int clientSocket,
				const ServerConfig& serverConfig,
				std::function<void (Request&, Response&)> processFn
			);
			Connection(const Connection&) = delete;
			Connection& operator=(const Connection&) = delete;

			void readRequest(std::uint8_t *buffer, ssize_t size);
			void sendResponse();
			void close();
			void onClose(std::function<void ()> cleanupFn);

			bool isClosed() const;

		private:
			int _clientSocket;
			int _msTimeout { 5000 };
			const ServerConfig& _serverConfig;
			Request _request { Request::Status::INCOMPLETE };
			std::vector<std::uint8_t> _requestBuffer;
			std::queue<std::pair<Request, Response>> _processedQueue;
			std::function<void (Request&, Response&)> _processFn;
			std::function<void ()> _cleanupFn;
			std::chrono::steady_clock::time_point _lastReceived;

			bool _isTimedOut() const;
			void _processBuffer();
			void _handleHeader();
			void _handleChunkedBody();
			void _handleBody();
	};
}
