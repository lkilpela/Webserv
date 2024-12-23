#pragma once

#include <cstddef>

namespace http {
	class ResponseBody {
		public:
			explicit ResponseBody(int clientSocket);
			virtual ~ResponseBody() = default;

			virtual bool send() = 0;

			const std::size_t getSize() const;

		protected:
			int _clientSocket;
			std::size_t _totalBytes { 0 };
			std::size_t _bytesSent { 0 };
	};
}
