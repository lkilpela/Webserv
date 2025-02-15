#pragma once

#include <string>
#include <array>
#include <unordered_map>
#include "constants.hpp"

namespace http {
	struct UploadFile {
		std::string name;
		std::string contentType;
		std::vector<uint8_t> data;
	};

	constexpr const char* getMimeType(const std::string &extension) {
		if (extension == "aac") return "audio/aac";
		if (extension == "abw") return "application/x-abiword";
		if (extension == "apng") return "image/apng";
		if (extension == "arc") return "application/x-freearc";
		if (extension == "avif") return "image/avif";
		if (extension == "avi") return "video/x-msvideo";
		if (extension == "bin") return "application/octet-stream";
		if (extension == "bmp") return "image/bmp";
		if (extension == "bz") return "application/x-bzip";
		if (extension == "bz2") return "application/x-bzip2";
		if (extension == "cda") return "application/x-cdf";
		if (extension == "csh") return "application/x-csh";
		if (extension == "css") return "text/css; charset=utf-8";
		if (extension == "csv") return "text/csv; charset=utf-8";
		if (extension == "doc") return "application/msword";
		if (extension == "epub") return "application/epub+zip";
		if (extension == "gz") return "application/gzip";
		if (extension == "gif") return "image/gif";
		if (extension == "htm" || extension == "html") return "text/html; charset=utf-8";
		if (extension == "jar") return "application/java-archive";
		if (extension == "jpeg" || extension == "jpg") return "image/jpeg";
		if (extension == "js") return "text/javascript; charset=utf-8";
		if (extension == "json") return "application/json; charset=utf-8";
		if (extension == "jsonld") return "application/ld+json; charset=utf-8";
		if (extension == "mid" || extension == "midi") return "audio/midi";
		if (extension == "mjs") return "text/javascript; charset=utf-8";
		if (extension == "mp3") return "audio/mpeg";
		if (extension == "mp4") return "video/mp4";
		if (extension == "mpeg") return "video/mpeg";
		if (extension == "oga") return "audio/ogg";
		if (extension == "ogv") return "video/ogg";
		if (extension == "ogx") return "application/ogg";
		if (extension == "opus") return "audio/ogg";
		if (extension == "otf") return "font/otf";
		if (extension == "png") return "image/png";
		if (extension == "pdf") return "application/pdf";
		if (extension == "php") return "application/x-httpd-php";
		if (extension == "rar") return "application/vnd.rar";
		if (extension == "rtf") return "application/rtf";
		if (extension == "sh") return "application/x-sh";
		if (extension == "svg") return "image/svg+xml";
		if (extension == "tar") return "application/x-tar";
		if (extension == "tif" || extension == "tiff") return "image/tiff";
		if (extension == "ts") return "video/mp2t";
		if (extension == "ttf") return "font/ttf";
		if (extension == "txt") return "text/plain; charset=utf-8";
		if (extension == "wav") return "audio/wav";
		if (extension == "weba") return "audio/webm";
		if (extension == "webm") return "video/webm";
		if (extension == "webp") return "image/webp";
		if (extension == "woff") return "font/woff";
		if (extension == "woff2") return "font/woff2";
		if (extension == "xhtml") return "application/xhtml+xml";
		if (extension == "xml") return "application/xml; charset=utf-8";
		if (extension == "zip") return "application/zip";
		if (extension == "7z") return "application/x-7z-compressed";
		return "text/plain; charset=utf-8";
	}

	constexpr const char* stringOf(Header header) {
		using enum Header;

		switch (header) {
			case CONTENT_TYPE: return "Content-Type";
			case CONTENT_ENCODING: return "Content-Encoding";
			case CONTENT_LANGUAGE: return "Content-Language";
			case CONTENT_LOCATION: return "Content-Location";
			case CONTENT_LENGTH: return "Content-Length";
			case CONTENT_RANGE: return "Content-Range";
			case TRAILER: return "Trailer";
			case TRANSFER_ENCODING: return "Transfer-Encoding";
			case CACHE_CONTROL: return "Cache-Control";
			case CONNECTION: return "Connection";
			case EXPECT: return "Expect";
			case HOST: return "Host";
			case MAX_FORWARDS: return "Max-Forwards";
			case PRAGMA: return "Pragma";
			case RANGE: return "Range";
			case TE: return "Te";
			case IF_MATCH: return "If-Match";
			case IF_NONE_MATCH: return "If-None-Match";
			case IF_MODIFIED_SINCE: return "If-Modified-Since";
			case IF_UNMODIFIED_SINCE: return "If-Unmodified-Since";
			case IF_RANGE: return "If-Range";
			case ACCEPT: return "Accept";
			case ACCEPT_CHARSET: return "Accept-Charset";
			case ACCEPT_ENCODING: return "Accept-Encoding";
			case ACCEPT_LANGUAGE: return "Accept-Language";
			case AUTHORIZATION: return "Authorization";
			case PROXY_AUTHORIZATION: return "Proxy-Authorization";
			case FROM: return "From";
			case REFERER: return "Referer";
			case USER_AGENT: return "User-Agent";
			case AGE: return "Age";
			case EXPIRES: return "Expires";
			case DATE: return "Date";
			case LOCATION: return "Location";
			case RETRY_AFTER: return "Retry-After";
			case VARY: return "Vary";
			case WARNING: return "Warning";
			case ETAG: return "Etag";
			case LAST_MODIFIED: return "Last-Modified";
			case WWW_AUTHENTICATE: return "Www-Authenticate";
			case PROXY_AUTHENTICATE: return "Proxy-Authenticate";
			case ACCEPT_RANGES: return "Accept-Ranges";
			case ALLOW: return "Allow";
			case SERVER: return "Server";
			case MIME_VERSION: return "Mime-Version";
			default: return "Unknown";
		}
	}

	constexpr const char* stringOf(StatusCode code) {
		using enum http::StatusCode;

		switch (code) {
			case CONTINUE_100: return "Continue";
			case SWITCHING_PROTOCOLS_101: return "Switching Protocols";
			case OK_200: return "OK";
			case CREATED_201: return "Created";
			case ACCEPTED_202: return "Accepted";
			case NON_AUTHORITATIVE_INFORMATION_203: return "Non-Authoritative Information";
			case NO_CONTENT_204: return "No Content";
			case RESET_CONTENT_205: return "Reset Content";
			case PARTIAL_CONTENT_206: return "Partial Content";
			case MULTIPLE_CHOICES_300: return "Multiple Choices";
			case MOVED_PERMANENTLY_301: return "Moved Permanently";
			case FOUND_302: return "Found";
			case SEE_OTHER_303: return "See Other";
			case NOT_MODIFIED_304: return "Not Modified";
			case TEMPORARY_REDIRECT_307: return "Temporary Redirect";
			case PERMANENT_REDIRECT_308: return "Permanent Redirect";
			case BAD_REQUEST_400: return "Bad Request";
			case UNAUTHORIZED_401: return "Unauthorized";
			case PAYMENT_REQUIRED_402: return "Payment Required";
			case FORBIDDEN_403: return "Forbidden";
			case NOT_FOUND_404: return "Not Found";
			case METHOD_NOT_ALLOWED_405: return "Method Not Allowed";
			case NOT_ACCEPTABLE_406: return "Not Acceptable";
			case PROXY_AUTHENTICATION_REQUIRED_407: return "Proxy Authentication Required";
			case REQUEST_TIMEOUT_408: return "Request Timeout";
			case CONFLICT_409: return "Conflict";
			case GONE_410: return "Gone";
			case LENGTH_REQUIRED_411: return "Length Required";
			case PRECONDITION_FAILED_412: return "Precondition Failed";
			case CONTENT_TOO_LARGE_413: return "Content Too Large";
			case URI_TOO_LONG_414: return "Uri Too Long";
			case UNSUPPORTED_MEDIA_TYPE_415: return "Unsupported Media Type";
			case RANGE_NOT_SATISFIABLE_416: return "Range Not Satisfiable";
			case EXPECTATION_FAILED_417: return "Expectation Failed";
			case UPGRADE_REQUIRED_426: return "Upgrade Required";
			case INTERNAL_SERVER_ERROR_500: return "Internal Server Error";
			case NOT_IMPLEMENTED_501: return "Not Implemented";
			case BAD_GATEWAY_502: return "Bad Gateway";
			case SERVICE_UNAVAILABLE_503: return "Service Unavailable";
			case GATEWAY_TIMEOUT_504: return "Gateway Timeout";
			case HTTP_VERSION_NOT_SUPPORTED_505: return "Http Version Not Supported";
			default: return "Unknown";
		}
	}

	bool hasHeaderName(const std::string &headerName);
	bool isValidHeaderField(const std::string &headerField);

	std::array<std::string, 3> parseRequestLine(const std::string &rawRequestHeader);
	std::unordered_map<std::string, std::string> parseRequestHeaderFields(const std::string &rawRequestHeader);
	Url parseUrl(const std::string& fullUrl);

	std::size_t parseChunkSize(std::string chunkSizeLine);
	bool unchunk(std::vector<uint8_t>& chunkedBuffer, std::vector<uint8_t>& result);
}
