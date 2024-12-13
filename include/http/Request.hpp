#pragma once

#include <string>
#include <vector>
#include <span>
#include <unordered_map>
#include "parse.hpp"
#include "Header.hpp"

namespace http {

	class Request {
		private:
			std::string _method;
			Uri _uri;
			std::string _version;
			std::unordered_map<std::string, std::string> _headers;
			std::vector<std::uint8_t> _buffer;
			std::string _filePath;
			bool _isCgi;
			bool _isDirectory;

		public:
			Request() = default;
			Request(const Request& request) = default;
			~Request() = default;
			Request& operator=(const Request& request) = default;

			void append(const char* data, size_t size);
			void Request::parse();

			const std::string& getMethod() const;
			const Uri& getUri() const;
			const std::string& getVersion() const;
			const std::span<const std::uint8_t> getBody() const;
			const std::string& getBodyAsFile() const;

			Request& setMethod(const std::string& method);
			Request& setUri(const Uri& uri);
			Request& setVersion(const std::string& version);
			Request& setHeader(Header header, const std::string& value);

			bool isDirectory() const;
			bool isCgi() const;
	};

}
