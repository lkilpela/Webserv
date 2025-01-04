#pragma once

#include <cstddef>
#include <string.h>
#include <filesystem>
#include <fstream>
#include <sstream> // std::ostringstream

namespace utils {
	class Payload {
		public:
			explicit Payload(int socket);
			Payload(Payload&&) noexcept = default;
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
			StringPayload(int socket, const std::string& message);
			StringPayload(StringPayload&&) noexcept = default;
			~StringPayload() = default;
			void send() override;
			void setMessage(const std::string& message);

			std::string toString() const override {
				std::ostringstream oss;
				oss << _message;
				return oss.str();
        	}

		private:
			std::string _message;
	};

	class FilePayload : public Payload {
		public:
			FilePayload(int socket, const std::filesystem::path& filePath);
			FilePayload(FilePayload&&) noexcept = default;
			~FilePayload() = default;

			void send() override;

			std::string toString() const override {
				std::ostringstream oss;
				oss << _ifstream.rdbuf();
				return oss.str();
        	}

		private:
			std::filesystem::path _filePath;
			mutable std::ifstream _ifstream; // mutable means it can be modified even in a const method
	};
}
