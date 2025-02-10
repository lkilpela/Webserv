#pragma once

#include <cstddef>
#include <string.h>
#include <filesystem>
#include <fstream>

namespace utils {
	class Payload {
		public:
			explicit Payload(int socket);
			Payload(const Payload&) = default;
			Payload(Payload &&) noexcept = default;
			virtual ~Payload() = default;

			Payload& operator=(const Payload&) = default;

			virtual void send() = 0;
			virtual void append(const std::uint8_t* data, size_t size) = 0;
			virtual std::string toString() const = 0;
			virtual std::unique_ptr<Payload> clone() const = 0;

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
			StringPayload(const StringPayload&) = default;
			StringPayload(StringPayload &&) noexcept = default;
			~StringPayload() = default;

			StringPayload& operator=(const StringPayload&) = default;

			void send() override;
			void append(const std::uint8_t* data, size_t size) override;
			std::string toString() const override;
			std::unique_ptr<Payload> clone() const override;

			void setMessage(const std::string &message);

		private:
			std::string _message;
	};

	class FilePayload : public Payload {
		public:
			FilePayload(int socket, const std::filesystem::path &filePath);
			FilePayload(const FilePayload& other);
			FilePayload(FilePayload &&) noexcept = default;
			~FilePayload() = default;

			FilePayload& operator=(const FilePayload& other);

			void send() override;
			void append(const std::uint8_t* data, size_t size) override;
			std::string toString() const override;
			std::unique_ptr<Payload> clone() const override;

		private:
			std::filesystem::path _filePath;
			mutable std::ifstream _ifstream;
	};
}
