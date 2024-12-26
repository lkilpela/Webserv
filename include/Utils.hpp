#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

namespace utils {
    std::string trim(const std::string& str);
    std::string removeComments(const std::string& str);
    bool parseBool(const std::string &value);
    int parsePort(const std::string &value);
    void validateMethods(const std::vector<std::string> &methods);
    bool isValidIP(const std::string &ip);
    bool isValidFilePath(const std::string &path);
    bool isValidURL(const std::string &url);
    bool isValidSize(const std::string &size);
    void validateErrorPage(const std::string &code, const std::string &path);

	class Payload {
		public:
			explicit Payload(int socket);
			virtual ~Payload() = default;
			virtual void send() = 0;
			bool isSent() const;

		protected:
			int _socket;
			std::size_t _totalBytes { 0 };
			std::size_t _bytesSent { 0 };
	};

	class StringPayload : public Payload {
		public:
			StringPayload(int socket, const std::string& message);
			~StringPayload() = default;
			void send() override;
			void setMessage(const std::string& message);

		private:
			std::string _message;
	};

	class FilePayload : public Payload {
		public:
			FilePayload(int socket, const std::filesystem::path& filePath);
			~FilePayload() = default;
			void send() override;

		private:
			std::filesystem::path _filePath;
			std::ifstream _ifstream;
	};
} // namespace utils
