#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "constants.hpp"
#include "ResponseBody.hpp"

namespace http {
	class Response {
		public:
			enum class Status { INCOMPLETE, READY, SENDING, SENT_ALL, ERROR };

			Response(int clientSocket);
			Response(const Response&) = delete;
			virtual ~Response() = default;

			Response& operator=(const Response&) = delete;

			bool send();
			void build();

			const Response::Status& getStatus() const;
			const http::StatusCode getHttpStatusCode() const;

			Response& setHttpStatusCode(const http::StatusCode statusCode);
			Response& setHeader(Header header, const std::string& value);
			Response& setBody(std::unique_ptr<ResponseBody> body);

		private:
			int _clientSocket;
			Response::Status _status { Response::Status::INCOMPLETE };
			http::StatusCode _statusCode { http::StatusCode::NONE };
			std::unordered_map<std::string, std::string> _headers;
			std::string _header;
			std::unique_ptr<ResponseBody> _body { nullptr };
			std::size_t _bytesSent { 0 };

			bool _sendHeader();
	};
}
