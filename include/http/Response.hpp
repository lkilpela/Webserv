#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "Status.hpp"
#include "Header.hpp"

namespace http {

	class Response {
		private:
			http::Status _status { Status::Code::ACCEPTED };
			std::string _type;
			std::string _url;
			std::string _body;
			std::unordered_map<std::string, std::string> _headers;
			std::size_t _totalBytesSent { 0 };
			bool _isRedirected { false };
			bool _isComplete { false };

			std::string _composeHeaders() const;
			void _send(int clientSocket, const void *buf, const size_t size, int flags);

		public:
			Response() = default;
			Response(const Response& response);
			~Response() = default;

			Response& operator=(const Response& response);

			const http::Status& getStatus() const;

			Response& setStatus(const http::Status& status);
			Response& setHeader(Header header, const std::string& value);
			Response& setBody(const std::string& bodyContent);

			void sendFile(const std::string& filePath);
			void sendStatus(int clientSocket, Status status);
			void sendText(const std::string& text);

			bool isComplete() const;
	};

}
