#include <array>
#include <iterator>
#include <sys/socket.h>
#include "utils/Payload.hpp"
#include "Error.hpp"

namespace utils {
	FilePayload::FilePayload(int socket, const std::filesystem::path &filePath)
		: Payload(socket),
		_filePath(filePath)
	{
		if (!std::filesystem::exists(filePath)) {
			throw FileNotFoundException(_filePath.filename());
		}

		_totalBytes = std::filesystem::file_size(filePath);
		_ifstream.open(filePath, std::ios::binary);

		if (!_ifstream.is_open()) {
			throw std::ios_base::failure("Failed to open " + filePath.string());
		}
	}

	void FilePayload::send() {
		if (Payload::_bytesSent >= _totalBytes) {
			return;
		}

		std::array<char, 1024> buffer;
		_ifstream.seekg(Payload::_bytesSent, std::ios::beg);
		const auto bytesRead = _ifstream
								.read(buffer.data(), buffer.size())
								.gcount();

		if (_ifstream.bad() || (_ifstream.fail() && !_ifstream.eof())) {
			_ifstream.close();
			throw std::ios_base::failure("Failed to read " + _filePath.string());
		}

		const ssize_t bytesSent = ::send(_socket, buffer.data(), bytesRead, MSG_NOSIGNAL);

		if (bytesSent >= 0) {
			Payload::_bytesSent += static_cast<std::size_t>(bytesSent);

			if (Payload::_bytesSent >= _totalBytes) {
				_ifstream.close();
			}
		}
	}

	void FilePayload::append(const std::uint8_t* data, size_t size) {
		throw std::runtime_error("The append() method is not supported in FilePayload");
	}

	std::string FilePayload::toString() const {
		_ifstream.seekg(0, std::ios::beg);
		return std::string(
			std::istreambuf_iterator<char>(_ifstream),
			std::istreambuf_iterator<char>()
		);
	}
}
