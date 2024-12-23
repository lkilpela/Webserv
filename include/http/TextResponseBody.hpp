#pragma once

#include <string>
#include "ResponseBody.hpp"

namespace http {
	class TextResponseBody : public ResponseBody {
		public:
			TextResponseBody(int clientSocket, const std::string& text);
			TextResponseBody(int clientSocket, std::string&& text);

			bool send() override;

			const std::string& getContent() const;

			TextResponseBody& setContent(const std::string& text);
			TextResponseBody& setContent(std::string&& text);

		private:
			std::string _content;
	};
}
