#pragma once

#include <cstddef>

namespace http {
	class Payload {
		public:
			explicit Payload(int socket);
			virtual ~Payload() = default;
			virtual void send() = 0;
			bool isSent() const;

		protected:
			int _socket;
			std::size_t _totalBytes { 0 };
			std::size_t _bytesSent { 0 };
	};
}
