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
			enum class Status : uint8_t {
				PENDING,		// The initial state of response, waiting to be processed.
				IN_PROGRESS,	// The response is being processed.
				READY			// The response is complete for delivery.
			};

			Response(int clientSocket);
			Response(const Response&) = delete;
			Response(Response&&) noexcept = default;
			~Response() = default;

			Response& operator=(const Response&) = delete;

			bool send();

			void build();

			int getClientSocket() const;
			Response::Status getStatus() const;
			StatusCode getStatusCode() const;
			const utils::StringPayload& getHeader() const;
			const std::unique_ptr<utils::Payload>& getBody() const;

			Response& clear();
			Response& setStatus(const Status status);
			Response& setStatusCode(const StatusCode statusCode);
			Response& setHeader(Header header, const std::string& value);
			Response& setBody(std::unique_ptr<utils::Payload> body);

			void appendBody(const std::uint8_t* data, size_t size);
			void setText(StatusCode statusCode, const std::string& text);
			void setFile(StatusCode statusCode, const std::filesystem::path &filePath);

		private:
			int _clientSocket;
			Status _status { Status::PENDING };
			StatusCode _statusCode { StatusCode::NONE_0 };
			std::unordered_map<std::string, std::string> _headerByName;
			utils::StringPayload _header;
			std::unique_ptr<utils::Payload> _body;
	};
}
