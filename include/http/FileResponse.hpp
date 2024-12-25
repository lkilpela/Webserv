#pragma once

#include <fstream>
#include <filesystem>
#include "Response.hpp"

namespace http {
	class FileResponse : public Response {
		public:
			FileResponse(int clientSocket, const std::filesystem::path& path);

			bool send() override;

			void build() override;

			const std::filesystem::path& getPath() const;

			FileResponse& setPath(const std::filesystem::path& path);

		private:
			std::filesystem::path _path;
			std::ifstream _ifstream;
			std::size_t _totalBytes;
			std::size_t _bytesSent;
	};
}
