#include <array>
#include <sys/socket.h>
#include "http/TextResponseBody.hpp"

namespace http {
	TextResponseBody::TextResponseBody(int clientSocket, const std::string& text)
		: ResponseBody(clientSocket)
		, _content(text)
	{
		_totalBytes = _content.size();
	}

	TextResponseBody::TextResponseBody(int clientSocket, std::string&& text)
		: ResponseBody(clientSocket)
		, _content(std::move(text))
	{
		_totalBytes = _content.size();
	}

	bool TextResponseBody::send() {
		const ssize_t bytesSent = ::send(
			_clientSocket,
			_content.data() + ResponseBody::_bytesSent,
			_totalBytes - ResponseBody::_bytesSent,
			MSG_NOSIGNAL
		);

		if (bytesSent >= 0) {
			ResponseBody::_bytesSent += static_cast<std::size_t>(bytesSent);

			if (ResponseBody::_bytesSent >= _totalBytes)
				return true;
		}

		return false;
	}

	const std::string& TextResponseBody::getContent() const {
		return _content;
	}

	TextResponseBody& TextResponseBody::setContent(const std::string& text) {
		_content = text;
		return *this;
	}

	TextResponseBody& TextResponseBody::setContent(std::string&& text) {
		_content = std::move(text);
		return *this;
	}
}
