#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include "Uri.hpp"

namespace http {
	class Request {
		private:
			int _clientSocket;
			std::string _method;
			http::Uri _uri;
			std::string _version;
			std::unordered_map<std::string, std::string> _headers;
			std::string _body;
			bool _isCgi;
			bool _isDirectory;

		public:
			Request(int clientSocket);
			Request(const Request& request) = default;
			~Request() = default;
			Request& operator=(const Request& request) = default;

			const std::string& getMethod() const;
			const http::Uri& getUri() const;
			const std::string& getVersion() const;
			const std::unordered_map<std::string, std::string>& getHeaders() const;
			std::optional<std::string> getHeader(const std::string& name) const;
			const std::string& getBody() const;

			Request& setMethod(const std::string& method);
			Request& setUri(const http::Uri& uri);
			Request& setVersion(const std::string& version);
			Request& setHeader(const std::string& name, const std::string& value);
			Request& setBody(const std::string& bodyContent);

			bool isDirectory() const;
			bool isCgi() const;

			// static std::variant<Request, HttpError> create(int clientSocket);
	};
}
