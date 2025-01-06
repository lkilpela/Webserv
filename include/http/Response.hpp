#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "constants.hpp"
#include "utils/Payload.hpp"

namespace http {
	class Response {
		public:
			Response(int clientSocket);
			Response(const Response&) = delete;
			Response(Response&&) noexcept = default;
			~Response() = default;

			Response& operator=(const Response&) = delete;

			bool send();

			void build();

			int getClientSocket() const;
			const StatusCode getStatusCode() const;

			Response& clear();
			Response& setStatusCode(const StatusCode statusCode);
			Response& setHeader(Header header, const std::string& value);
			Response& setBody(std::unique_ptr<utils::Payload> body);

			std::string getBody() const {
				if (_body) {
					return _body->toString();
				}
				return "";
			} 

			// Function to set the response for string payloads
			void setStringResponse(Response& res, StatusCode statusCode, const std::string& body);

			// Function to set the response for file payloads
			void setFileResponse(Response& res, StatusCode statusCode, const std::string& filePath);

		private:
			//int _clientSocket;
			StatusCode _statusCode { StatusCode::NONE_0 };
			std::unordered_map<std::string, std::string> _headerByName;
			utils::StringPayload _header;
			std::unique_ptr<utils::Payload> _body;
	};
}
