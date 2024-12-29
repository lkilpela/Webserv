#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <span>
#include <unordered_map>
#include "Url.hpp"
#include "constants.hpp"

namespace http {
	class Request {
		public:
			enum class Status {
				INCOMPLETE,			// The request is not yet processed.
				HEADER_COMPLETE,	// The request has only headers processed.
				BAD,				// The request is malformed or invalid.
				COMPLETE			// The request is fully processed.
			};

			Request() = default;
			explicit Request(Status status);
			Request(const Request&) = delete;
			Request(Request&&) noexcept = default;
			~Request() = default;
			Request& operator=(const Request&) = delete;

			void clear();

			bool isChunked() const;

			const std::string& getMethod() const;
			const Url& getUrl() const;
			const std::string& getVersion() const;
			std::optional<std::string> getHeader(Header header) const;
			std::size_t getBodySize() const;
			const std::span<const std::uint8_t> getBody() const;
			Request::Status getStatus() const;

			Request& setMethod(const std::string& method);
			Request& setUrl(const Url& url);
			Request& setVersion(const std::string& version);
			Request& setHeader(Header header, const std::string& value);
			Request& setHeader(const std::string& name, const std::string& value);
			Request& appendBody(std::vector<uint8_t>::iterator begin, std::vector<uint8_t>::iterator end) noexcept;
			Request& setStatus(Request::Status status);

			static bool parseHeaders(Request& request, const std::string& rawHeader);

		private:
			std::string _method;
			Url _url;
			std::string _version;
			std::unordered_map<std::string, std::string> _headers;
			std::vector<std::uint8_t> _body;
			std::size_t _bodySize { 0 };
			Request::Status _status { Request::Status::INCOMPLETE };
	};
}
