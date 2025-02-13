#include <sstream>
#include <cstring>
#include <array>
#include <algorithm>
#include <sys/socket.h>
#include "utils/index.hpp"
#include "http/Request.hpp"
#include "http/utils.hpp"

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

	std::size_t Request::getContentLength() const {
		return _contentLength;
	}

	const std::span<const std::uint8_t> Request::getBody() const {
		return std::span<const std::uint8_t>(_body);
	}

	Request::Status Request::getStatus() const {
		return _status;
	}

	Request& Request::appendBody(std::vector<uint8_t>::iterator begin, std::vector<uint8_t>::iterator end) noexcept {
		_body.insert(_body.end(), std::move_iterator(begin), std::move_iterator(end));
		return *this;
	}

	Request& Request::setContentLength(std::size_t bytes) {
		_contentLength = bytes;
		return *this;
	}

	Request& Request::setHeader(const std::string& name, const std::string& value) {
		_headers[name] = value;
		return *this;
	}

	Request& Request::setHeader(Header header, const std::string& value) {
		_headers[stringOf(header)] = value;
		return *this;
	}

	Request& Request::setMethod(const std::string& method) {
		_method = method;
		return *this;
	}

	Request& Request::setStatus(Request::Status status) {
		_status = status;
		return *this;
	}

	Request& Request::setUrl(const Url& url) {
		_url = url;
		return *this;
	}

	Request& Request::setUrl(Url&& url) {
		_url = std::move(url);
		return *this;
	}

	Request& Request::setVersion(const std::string& version) {
		_version = version;
		return *this;
	}

	/**
	 * @brief Parse the given string request header into a http::Request object
	 *
	 * @param rawRequestHeader The http request header in string format.
	 *
	 * @return http::Request request
	 *
	 * @throw std::invalid_argument
	 */
	Request Request::parseHeader(const std::string& rawRequestHeader) {
		Request request;

		const auto& [method, uri, version] = parseRequestLine(rawRequestHeader);
		const auto& headersMap = parseRequestHeaders(rawRequestHeader);
		Url url = Url::parse(headersMap.at(stringOf(Header::HOST)) + uri);

		for (const auto& [name, value] : headersMap) {
			if (name == stringOf(Header::CONTENT_LENGTH)) {
				request.setContentLength(std::stoul(value));
			}

			if (name == stringOf(Header::TRANSFER_ENCODING) && value == "chunked" && method == "GET") {
				throw std::invalid_argument("Chunked transfer encoding is not allowed in GET requests");
			}

			request.setHeader(name, value);
		}

		request
			.setMethod(method)
			.setUrl(std::move(url))
			.setVersion(version)
			.setStatus(Status::HEADER_COMPLETE);
		return request;
	}

	// void Request::parseMultipart(
	// 	std::vector<uint8_t>& rawRequestBody,
	// 	std::vector<UploadFile>& result,
	// 	const std::string& boundary
	// ) {
	// 	const std::string finalBoundary(boundary + "--\r\n");
	// 	const std::string nameDelim("filename=");
	// 	auto begin = rawRequestBody.begin();
	// 	auto end = rawRequestBody.end();

	// 	auto endPos = std::search(begin, end, finalBoundary.begin(), finalBoundary.end());

	// 	if (endPos == end) {
	// 		return;
	// 	}

	// 	auto currentPos = begin;

	// 	while (currentPos != endPos) {
	// 		currentPos = std::search(currentPos, end, nameDelim.begin(), nameDelim.end());

	// 		UploadFile uploadFile;

	// 		result.push_back(uploadFile);
	// 		currentPos += 2;
	// 	}
	// }

	std::size_t Request::BodyParser::parseChunkSize(std::string chunkSizeLine) {
		std::size_t semicolonPos = chunkSizeLine.find(";");

		if (semicolonPos != std::string::npos) {
			chunkSizeLine = chunkSizeLine.substr(0, semicolonPos);
		}

		std::size_t chunkSize;
		std::istringstream stream(chunkSizeLine);

		if (!(stream >> std::hex >> chunkSize)) {
			throw std::invalid_argument("Failed to parse chunk size");
		}

		return chunkSize;
	}

	bool Request::BodyParser::parseChunk(std::vector<uint8_t>& rawRequestBody, std::vector<uint8_t>& result) {
		bool isChunkEnd = false;
		auto begin = rawRequestBody.begin();
		auto end = rawRequestBody.end();
		auto currentPos = begin;

		result.reserve(rawRequestBody.size());

		while (true) {
			auto delimPos = utils::findDelimiter(currentPos, end, {'\r', '\n'});

			if (delimPos == end || std::distance(delimPos, end) < 2) {
				break;
			}

			std::size_t chunkSize = parseChunkSize(std::string(currentPos, delimPos));
			std::size_t distance = static_cast<std::size_t>(std::distance(delimPos + 2, end));

			if (distance < chunkSize + 2) {
				break;
			}

			currentPos = delimPos + 2;

			if (chunkSize == 0) {
				if (*currentPos == '\r' && *(currentPos + 1) == '\n') {
					isChunkEnd = true;
					currentPos += 2;
					break;
				}

				throw std::invalid_argument(R"(Error: Chunk body did not end with 0\r\n\r\n)");
			}

			result.insert(result.end(), currentPos, currentPos + chunkSize);
			currentPos += chunkSize + 2;
		}

		rawRequestBody.erase(begin, currentPos);
		return isChunkEnd;
	}
}
