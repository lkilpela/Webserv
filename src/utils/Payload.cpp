#include "Utils.hpp"

namespace utils {
	Payload::Payload(int socket) : _socket(socket) {}

	bool Payload::isSent() const {
		return (_bytesSent >= _totalBytes);
	}
}
