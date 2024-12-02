#pragma once

#include <string>
#include <unordered_map>

class HttpRequest {
	private:
		std::string _method;
    	std::string _url;
    	std::unordered_map<std::string, std::string> _headers;
    	std::string _body;

	public:
		HttpRequest(const std::string& rawRequest);
		HttpRequest(const HttpRequest& request);
		~HttpRequest();
		HttpRequest& operator=(const HttpRequest& request);

		const std::string& getMethod() const;
		const std::string& getUrl() const;
		const std::unordered_map<std::string, std::string>& getHeaders() const;
		const std::string& getBody() const;
};
