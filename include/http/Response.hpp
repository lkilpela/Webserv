#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <filesystem>
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
			/* const  */StatusCode getStatusCode() const;
			//const StatusCode getStatusCode() const;
			const utils::StringPayload& getHeader() const;
			const std::unique_ptr<utils::Payload>& getBody() const;

			Response& clear();
			Response& setStatusCode(const StatusCode statusCode);
			Response& setHeader(Header header, const std::string& value);
			Response& setBody(std::unique_ptr<utils::Payload> body);

			void setFile(StatusCode statusCode, const std::filesystem::path &filePath);

		private:
			int _clientSocket;
			StatusCode _statusCode { StatusCode::NONE_0 };
			std::unordered_map<std::string, std::string> _headerByName;
			utils::StringPayload _header;
			std::unique_ptr<utils::Payload> _body;
	};
}
