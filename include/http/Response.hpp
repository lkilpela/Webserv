#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "constants.hpp"
#include "Payload.hpp"

namespace http {
	class Response {
		public:
			Response(int clientSocket);
			Response(const Response&) = delete;
			virtual ~Response() = default;

			Response& operator=(const Response&) = delete;

			virtual bool send();

			virtual void build();

			const StatusCode getStatusCode() const;

			virtual Response& clear();
			Response& setStatusCode(const StatusCode statusCode);
			Response& setHeader(Header header, const std::string& value);
			Response& setBody(std::unique_ptr<Payload> body);

		protected:
			int _clientSocket;
			StatusCode _statusCode { StatusCode::NONE_0 };
			std::unordered_map<std::string, std::string> _headerByName;
			std::string _header;
			std::unique_ptr<Payload> _body;
			std::size_t _bytesSent { 0 };

	};
}
