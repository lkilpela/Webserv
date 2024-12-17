#pragma once

#include <poll.h>
#include <chrono>
#include "Request.hpp"
#include "Response.hpp"

namespace http {

	class Connection {

		private:
			struct pollfd _pollFd;
			int _timeout_ms;
			Request _request;
			Response _response;
			std::chrono::steady_clock::time_point _lastReceived;

		public:
			Connection(struct pollfd& pollFd, int timeout_ms);
			const bool isTimeout() const;
			void close();
	};
}