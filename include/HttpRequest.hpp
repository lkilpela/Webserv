#pragma once

#include <string>
#include <unordered_map>

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
		HttpRequest(const std::string& rawRequest);
		HttpRequest(const HttpRequest& request);
		~HttpRequest() = default;
		HttpRequest& operator=(const HttpRequest& request);

		const std::string& getMethod() const;
		const std::string& getUri() const;
		const std::string& getVersion() const;
		const std::unordered_map<std::string, std::string>& getHeaders() const;
		const std::string& getBody() const;

		bool isDirectory() const;
		bool isCgi() const;
};
