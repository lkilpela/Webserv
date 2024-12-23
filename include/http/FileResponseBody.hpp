#pragma once

#include <fstream>
#include <filesystem>
#include "ResponseBody.hpp"

namespace http {
	class FileResponseBody : public ResponseBody {
		public:
			FileResponseBody(int clientSocket, const std::filesystem::path& path);

			bool send() override;

			const std::filesystem::path& getPath() const;

			FileResponseBody& setPath(const std::filesystem::path& path);

		private:
			std::filesystem::path _path;
			std::ifstream _ifstream;
	};
}
