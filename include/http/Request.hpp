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
			enum class Status { COMPLETE, INCOMPLETE, BAD_REQUEST };

			Request() = default;
			explicit Request(Status status);
			Request(const Request&) = delete;
			Request(Request&&) noexcept = default;
			~Request() = default;
			Request& operator=(const Request&) = delete;

			void clear();

			const std::string& getMethod() const;
			const Url& getUrl() const;
			const std::string& getVersion() const;
			std::optional<std::string> getHeader(Header header) const;
			const std::span<const std::uint8_t> getBody() const;
			Request::Status getStatus() const;

			Request& setMethod(const std::string& method);
			Request& setUrl(const Url& url);
			Request& setVersion(const std::string& version);
			Request& setHeader(Header header, const std::string& value);
			Request& setStatus(Request::Status status);

		private:
			std::string _method;
			Url _url;
			std::string _version;
			std::unordered_map<std::string, std::string> _headers;
			std::vector<std::uint8_t> _body;
			Request::Status _status { Request::Status::INCOMPLETE };
	};
}
