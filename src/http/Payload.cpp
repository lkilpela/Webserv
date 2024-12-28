
#include "Payload.hpp"

namespace http {
	Payload::Payload(int socket) : _socket(socket) {}

	bool Payload::isSent() const {
		return (_bytesSent >= _totalBytes);
	}
};
