#pragma once

#include <cstddef>
#include <string.h>
#include <filesystem>
#include <fstream>

namespace utils {
	class Payload {
		public:
			explicit Payload(int socket);
			Payload(Payload &&) noexcept = default;
			virtual ~Payload() = default;

			virtual void send() = 0;
			virtual std::string toString() const = 0;

			bool isSent() const;
			std::size_t getSizeInBytes() const;

		protected:
			int _socket;
			std::size_t _totalBytes { 0 };
			std::size_t _bytesSent { 0 };
	};

	class StringPayload : public Payload {
		public:
			StringPayload(int socket, const std::string &message);
			StringPayload(StringPayload &&) noexcept = default;
			~StringPayload() = default;

			void send() override;
			std::string toString() const override;

			void setMessage(const std::string &message);

		private:
			std::string _message;
	};

	class FilePayload : public Payload {
		public:
			FilePayload(int socket, const std::filesystem::path &filePath);
			FilePayload(FilePayload &&) noexcept = default;
			~FilePayload() = default;

			void send() override;
			std::string toString() const override;

		private:
			std::filesystem::path _filePath;
			mutable std::ifstream _ifstream;
	};
}
