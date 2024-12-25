#pragma once

#include <string>
#include <unordered_map>
#include "constants.hpp"

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

		protected:
			int _clientSocket;
			StatusCode _statusCode { StatusCode::NONE_0 };
			std::unordered_map<std::string, std::string> _headerByName;
			std::string _header;
			std::size_t _bytesSent { 0 };

	};
}
