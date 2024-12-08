#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "Status.hpp"

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

		public:
			Response(int clientSocket);
			Response(const Response& response);
			~Response() = default;

			Response& operator=(const Response& response);

			const http::Status& getStatus() const;
			const std::unordered_map<std::string, std::string>& getHeaders() const;

			Response& setStatus(const http::Status& status);
			Response& setHeader(const std::string& name, const std::string& value);
			Response& setBody(const std::string& bodyContent);

			void sendFile(const std::string& filePath, std::function<void(std::exception&)> onError);
			std::string toString() const;

	};
}
