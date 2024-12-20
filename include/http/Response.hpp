#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "Status.hpp"
#include "Header.hpp"

namespace http {

	class Response {
		public:
			enum class Status { INCOMPLETE, READY, SENDING, SENT, ERROR };

			Response(int clientSocket);
			Response(const Response&) = delete;
			~Response() = default;

			Response& operator=(const Response&) = delete;

			const http::Status& getHttpStatus() const;

			Response& setHttpStatus(const http::Status& httpStatus);
			Response& setHeader(Header header, const std::string& value);
			Response& setBody(const std::string& bodyContent);

			void sendFile(const std::string& filePath);
			// void sendStatus(int clientSocket, Status status);
			// void sendText(const std::string& text);

		private:
			int _clientSocket;
			Response::Status _status { Response::Status::INCOMPLETE };
			http::Status _httpStatus { http::Status::Code::NONE };
			std::string _body;
			std::unordered_map<std::string, std::string> _headers;
			std::size_t _bytesSent { 0 };
			std::size_t _totalBytes { 0 };

			std::string _composeHeaders() const;
			ssize_t _send();

	};

}
