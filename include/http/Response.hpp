#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "constants.hpp"

namespace http {
	class Response {
		public:
			enum class Status { INCOMPLETE, READY, SENDING, SENT_ALL, ERROR };

			Response(int clientSocket);
			Response(const Response&) = delete;
			~Response() = default;

			Response& operator=(const Response&) = delete;

			const Response::Status& getStatus() const;
			const http::StatusCode getHttpStatusCode() const;

			Response& setHttpStatusCode(const http::StatusCode statusCode);
			Response& setHeader(Header header, const std::string& value);
			Response& setBody(const std::string& bodyContent);
			Response& setFile(const std::string& filePath);

			void send();
			void build();
			// void sendStatus(int clientSocket, Status status);
			// void sendText(const std::string& text);

		private:
			int _clientSocket;
			Response::Status _status { Response::Status::INCOMPLETE };
			http::StatusCode _statusCode { http::StatusCode::NONE };
			std::unordered_map<std::string, std::string> _headers;
			std::string _header;
			std::string _body;
			std::string _filePath;
			std::ifstream _fileStream;

			void _sendBody();
	};
}
