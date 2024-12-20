#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>
#include <unordered_map>
#include "Url.hpp"
#include "Header.hpp"

namespace http {

	class Request {
		public:
			enum class Status { COMPLETE, INCOMPLETE, BAD_REQUEST };

			Request() = default;
			Request(const Request&) = delete;
			Request(Request&& request);
			~Request() = default;
			Request& operator=(const Request&) = delete;

			const std::string& getMethod() const;
			const Url& getUrl() const;
			const std::string& getVersion() const;
			const std::span<const std::uint8_t> getBody() const;
			const std::string& getBodyAsFile() const;
			const Request::Status& getStatus() const;

			Request& setMethod(const std::string& method);
			Request& setUrl(const Url& url);
			Request& setVersion(const std::string& version);
			Request& setHeader(Header header, const std::string& value);
			Request& setStatus(const Request::Status& status);

			bool isCgi() const;
			bool isDirectory() const;

			template <typename Iterator>
			static Request parse(Iterator begin, Iterator end) {

			}

		private:
			std::string _method;
			Url _url;
			std::string _version;
			std::unordered_map<std::string, std::string> _headers;
			std::vector<std::uint8_t> _body;
			std::string _filePath;
			Request::Status _status { Request::Status::INCOMPLETE };
			bool _isCgi { false };
			bool _isDirectory { false };
	};

}
