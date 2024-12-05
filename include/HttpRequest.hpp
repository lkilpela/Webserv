#pragma once

#include <string>
#include <unordered_map>
#include <variant>

struct ParseError {
	int errorCode;
	std::string errorMessage;

	ParseError(int code, std::string& msg) : errorCode(code), errorMessage(msg) {}
};

class HttpRequest {
	private:
		std::string _method;
    	std::string _uri;
    	std::string _version;
    	std::unordered_map<std::string, std::string> _headers;
    	std::string _body;
		bool _isCgi;
		bool _isDirectory;

	public:
		HttpRequest() = default;
		HttpRequest(const HttpRequest& request);
		~HttpRequest() = default;
		HttpRequest& operator=(const HttpRequest& request);

		const std::string& getMethod() const;
		const std::string& getUri() const;
		const std::string& getVersion() const;
		const std::unordered_map<std::string, std::string>& getHeaders() const;
		const std::string& getBody() const;

		void setHeader(const std::string& name, const std::string& value);

		bool isDirectory() const;
		bool isCgi() const;

		static std::variant<HttpRequest, ParseError> parse(const std::string& rawRequest);
};
