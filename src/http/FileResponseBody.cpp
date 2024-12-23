#include <array>
#include <sys/socket.h>
#include "Error.hpp"
#include "http/FileResponseBody.hpp"

namespace fs = std::filesystem;

namespace http {
	FileResponseBody::FileResponseBody(int clientSocket, const fs::path& path)
		: ResponseBody(clientSocket)
		, _path(path)
	{
		if (!fs::exists(path)) {
			throw FileNotFoundException { path.filename() };
		}

		_totalBytes = fs::file_size(_path);
		_ifstream.open(path, std::ios::binary);

		if (!_ifstream.is_open()) {
			throw std::ios_base::failure("Failed to open " + path.string());
		}
	}

	bool FileResponseBody::send() {
		if (ResponseBody::_bytesSent >= _totalBytes) {
			return true;
		}

		std::array<char, 1024> buffer;
		_ifstream.seekg(ResponseBody::_bytesSent, std::ios::beg);
		const auto bytesRead = _ifstream
								.read(buffer.data(), buffer.size())
								.gcount();

		if (_ifstream.bad() || (_ifstream.fail() && !_ifstream.eof())) {
			_ifstream.close();
			throw std::ios_base::failure("Failed to read " + _path.string());
		}

		const ssize_t bytesSent = ::send(_clientSocket, buffer.data(), bytesRead, MSG_NOSIGNAL);

		if (bytesSent >= 0) {
			ResponseBody::_bytesSent += static_cast<std::size_t>(bytesSent);

			if (ResponseBody::_bytesSent >= _totalBytes) {
				_ifstream.close();
				return true;
			}
		}

		return false;
	}

	const fs::path& FileResponseBody::getPath() const {
		return _path;
	}

	FileResponseBody& FileResponseBody::setPath(const fs::path& path) {
		_path = path;
		return *this;
	}
}
