#include <array>
#include <sys/socket.h>
#include "utils/Payload.hpp"
#include "Error.hpp"

namespace utils {
	StringPayload::StringPayload(int socket, const std::string& message) : Payload(socket), _message(message) {
		_totalBytes = message.size();
	}

	void StringPayload::send() {
		if (Payload::_bytesSent >= _totalBytes) {
			return;
		}

		const ssize_t bytesSent = ::send(
			_socket,
			_message.data() + Payload::_bytesSent,
			_totalBytes - Payload::_bytesSent,
			MSG_NOSIGNAL
		);

		if (bytesSent >= 0) {
			Payload::_bytesSent += static_cast<std::size_t>(bytesSent);
		}
	}

	void StringPayload::append(const std::uint8_t* data, size_t size) {
		_message.append(reinterpret_cast<const char*>(data), size);
	}

	std::string StringPayload::toString() const {
		return _message;
	}

	void StringPayload::setMessage(const std::string& message) {
		_message = message;
	}
}
