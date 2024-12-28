#include <array>
#include <sys/socket.h>
#include "Error.hpp"
#include "http/FileResponse.hpp"

namespace fs = std::filesystem;

namespace http {
	FileResponse::FileResponse(int clientSocket, const fs::path& path) : Response(clientSocket), _path(path) {}

	bool FileResponse::send() {
		Response::send();

		std::array<char, 1024> buffer;
		_ifstream.seekg(this->_bytesSent, std::ios::beg);
		const auto bytesRead = _ifstream
								.read(buffer.data(), buffer.size())
								.gcount();

		if (_ifstream.bad() || (_ifstream.fail() && !_ifstream.eof())) {
			_ifstream.close();
			throw std::ios_base::failure("Failed to read " + _path.string());
		}

		const ssize_t bytesSent = ::send(_clientSocket, buffer.data(), bytesRead, MSG_NOSIGNAL);

		if (bytesSent >= 0) {
			this->_bytesSent += static_cast<std::size_t>(bytesSent);

			if (this->_bytesSent >= _totalBytes) {
				_ifstream.close();
				return true;
			}
		}

		return false;
	}

	void FileResponse::build() {
		Response::build();

		if (!fs::exists(_path)) {
			throw FileNotFoundException { _path.filename() };
		}

		_totalBytes = fs::file_size(_path);
		_ifstream.open(_path, std::ios::binary);

		if (!_ifstream.is_open()) {
			throw std::ios_base::failure("Failed to open " + _path.string());
		}
	}

	const fs::path& FileResponse::getPath() const {
		return _path;
	}

	FileResponse& FileResponse::setPath(const fs::path& path) {
		_path = path;
		return *this;
	}
}
