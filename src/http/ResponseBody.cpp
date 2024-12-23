#include "ResponseBody.hpp"

namespace http {
	ResponseBody::ResponseBody(int clientSocket) : _clientSocket(clientSocket) {}

	const std::size_t ResponseBody::getSize() const {
		return _totalBytes;
	}
}
