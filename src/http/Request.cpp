#include <sstream>
#include <cstring>
#include <array>
#include <algorithm>
#include <sys/socket.h>
#include "utils.hpp"
#include "http/Request.hpp"

// Behavior of recv()
// When recv() returns:
// > 0: Data was successfully received.
// 0: The peer has performed an orderly shutdown (connection closed).
// < 0: An error occurred. If the error is EAGAIN or EWOULDBLOCK, recv() does not block and returns -1 immediately.
// Approach
// To detect non-blocking cases (EAGAIN/EWOULDBLOCK):

// If recv() returns -1 and the socket is non-blocking, assume it’s a temporary error and retry when POLLIN is triggered again.

namespace http {

	Request::Request(Status status) : _status(status) {}

	void Request::clear() {
		_method.clear();
		_url = Url();
		_version.clear();
		_headers.clear();
		_body.clear();
		_status = Request::Status::INCOMPLETE;
	}

	bool Request::isChunked() const {
		auto header = getHeader(Header::TRANSFER_ENCODING);
		return (header.has_value() && *header == "chunked");
	}

	const std::string& Request::getMethod() const {
		return _method;
	}

	const Url& Request::getUrl() const {
		return _url;
	}

	const std::string& Request::getVersion() const {
		return _version;
	}

	std::optional<std::string> Request::getHeader(Header header) const {
		auto it = _headers.find(stringOf(header));

		if (it == _headers.end()) {
			return std::nullopt;
		}

		return it->second;
	}

	const std::span<const std::uint8_t> Request::getBody() const {
		return std::span<const std::uint8_t>(_body);
	}

	Request::Status Request::getStatus() const {
		return _status;
	}

	Request& Request::setMethod(const std::string& method) {
		_method = method;
		return *this;
	}

	Request& Request::setUrl(const Url& url) {
		_url = url;
		return *this;
	}

	Request& Request::setVersion(const std::string& version) {
		_version = version;
		return *this;
	}

	Request& Request::setHeader(Header header, const std::string& value) {
		_headers[stringOf(header)] = value;
		return *this;
	}

	Request& Request::setHeader(const std::string& name, const std::string& value) {
		_headers[name] = value;
		return *this;
	}

	Request& Request::appendBody(std::vector<uint8_t>::iterator begin, std::vector<uint8_t>::iterator end) noexcept {
		_body.insert(_body.end(), std::move_iterator(begin), std::move_iterator(end));
		return *this;
	}

	Request& Request::setStatus(Request::Status status) {
		_status = status;
		return *this;
	}

	bool Request::parseHeaders(Request& request, const std::string& rawHeader) {
		std::istringstream istream(rawHeader);
		std::string line;
		std::string method;
	 	Url url;
	 	std::string version;

		std::getline(istream, line);

		request.setMethod(method).setVersion(version);

		while (std::getline(istream, line) && line != "\r") {
			std::size_t delimiter = line.find(": ");

			if (delimiter == std::string::npos) {
				return false;
			}

			std::string name = line.substr(0, delimiter);
			// must validate `name`
			std::string value = line.substr(delimiter + 2);
			request.setHeader(name, value);
		}


		auto hostHeader = request.getHeader(Header::HOST);

		return true;
	}
}
