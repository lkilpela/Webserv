#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "Status.hpp"
#include "Header.hpp"

namespace http {

	class Response {
		private:
			int _clientSocket;
			http::Status _status { http::Status::Code::ACCEPTED };
			std::string _type;
			std::string _url;
			std::string _body;
			std::unordered_map<std::string, std::string> _headers;
			bool _isRedirected;

			std::string _composeHeaders() const;
			void _send(const void *buf, const size_t size, int flags);

		public:
			Response(int clientSocket);
			Response(const Response& response);
			~Response() = default;

			Response& operator=(const Response& response);

			const http::Status& getStatus() const;

			Response& setStatus(const http::Status& status);
			Response& setHeader(Header header, const std::string& value);
			Response& setBody(const std::string& bodyContent);

			void sendFile(const std::string& filePath, std::function<void(std::exception&)> onError);
			void sendStatus(http::Status status);
			void sendText(const std::string& text);
	};

}
