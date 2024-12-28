#include "utils/Payload.hpp"

namespace utils {
	Payload::Payload(int socket) : _socket(socket) {}

	bool Payload::isSent() const {
		return (_bytesSent >= _totalBytes);
	}

	std::size_t Payload::getSizeInBytes() const {
		return _totalBytes;
	}
}
