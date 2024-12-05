#pragma once

#include <string>
#include <unordered_map>

class HttpResponse {
	private:
		unsigned short int _statusCode;
		std::string _statusText;
		std::string _type;
		std::string _url;
		std::string _body;
		std::unordered_map<std::string, std::string> _headers;
		bool _isRedirected;

	public:
		enum class StatusCode {
			CONTINUE = 100,
			SWITCHING_PROTOCOLS = 101,
			OK = 200,
			CREATED = 201,
			ACCEPTED = 202,
			NON_AUTHORITATIVE_INFORMATION = 203,
			NO_CONTENT = 204,
			RESET_CONTENT = 205,
			PARTIAL_CONTENT = 206,
			MULTIPLE_CHOICES = 300,
			MOVED_PERMANENTLY = 301,
			FOUND = 302,
			SEE_OTHER = 303,
			NOT_MODIFIED = 304,
			TEMPORARY_REDIRECT = 307,
			PERMANENT_REDIRECT = 308,
			BAD_REQUEST = 400,
			UNAUTHORIZED = 401,
			PAYMENT_REQUIRED = 402,
			FORBIDDEN = 403,
			NOT_FOUND = 404,
			METHOD_NOT_ALLOWED = 405,
			NOT_ACCEPTABLE = 406,
			PROXY_AUTHENTICATION_REQUIRED = 407,
			REQUEST_TIMEOUT = 408,
			CONFLICT = 409,
			GONE = 410,
			LENGTH_REQUIRED = 411,
			PRECONDITION_FAILED = 412,
			CONTENT_TOO_LARGE = 413,
			URI_TOO_LONG = 414,
			UNSUPPORTED_MEDIA_TYPE = 415,
			RANGE_NOT_SATISFIABLE = 416,
			EXPECTATION_FAILED = 417,
			UPGRADE_REQUIRED = 426,
			INTERNAL_SERVER_ERROR = 500,
   			NOT_IMPLEMENTED = 501,
   			BAD_GATEWAY = 502,
   			SERVICE_UNAVAILABLE = 503,
   			GATEWAY_TIMEOUT = 504,
   			HTTP_VERSION_NOT_SUPPORTED = 505
		};

		HttpResponse();
		HttpResponse(StatusCode statusCode);

		unsigned short int getStatusCode() const;
		const std::string& getStatusText() const;
		const std::unordered_map<std::string, std::string>& getHeaders() const;

		void setStatus(StatusCode statusCode);
		void setHeader(const std::string& name, const std::string& value);
		void setBody(const std::string& bodyContent);

		std::string toString() const;

};
