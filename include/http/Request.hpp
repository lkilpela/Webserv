#pragma once

#include <cstdint>
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
			Url _url;
			std::string _version;
			std::unordered_map<std::string, std::string> _headers;
			std::vector<std::uint8_t> _buffer;
			std::string _filePath;
			bool _isComplete { false };
			bool _isCgi { false };
			bool _isDirectory { false };
			uint8_t _numberOfRetries { 0 };

		public:
			Request() = default;
			Request(const Request& request) = default;
			~Request() = default;
			Request& operator=(const Request& request) = default;

			void append(const char* data, size_t size);
			void parse();
			void reset();

			const std::string& getMethod() const;
			const Url& getUrl() const;
			const std::string& getVersion() const;
			const std::span<const std::uint8_t> getBody() const;
			const std::string& getBodyAsFile() const;

			Request& setMethod(const std::string& method);
			Request& setUrl(const Url& url);
			Request& setVersion(const std::string& version);
			Request& setHeader(Header header, const std::string& value);

			bool isComplete() const;
			bool isCgi() const;
			bool isDirectory() const;
	};

}
