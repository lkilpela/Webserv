#include "http/Header.hpp"

namespace http {

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

}
