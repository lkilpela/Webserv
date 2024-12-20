#pragma once

#include <string>

namespace http {
	enum class Header {
		// Representation
		CONTENT_TYPE,
		CONTENT_ENCODING,
		CONTENT_LANGUAGE,
		CONTENT_LOCATION,

		// Payload
		CONTENT_LENGTH,
   		CONTENT_RANGE,
   		TRAILER,
   		TRANSFER_ENCODING,

		// Controls
		CACHE_CONTROL,
		EXPECT,
		HOST,
		MAX_FORWARDS,
		PRAGMA,
		RANGE,
		TE,

		// Conditionals
		IF_MATCH,
		IF_NONE_MATCH,
		IF_MODIFIED_SINCE,
		IF_UNMODIFIED_SINCE,
		IF_RANGE,

		// Content Negotiation
		ACCEPT,
		ACCEPT_CHARSET,
		ACCEPT_ENCODING,
		ACCEPT_LANGUAGE,

		// Authentication Credentials
		AUTHORIZATION,
		PROXY_AUTHORIZATION,

		// Request Context
		FROM,
		REFERER,
		USER_AGENT,

		// Control Data
		AGE,
		EXPIRES,
		DATE,
		LOCATION,
		RETRY_AFTER,
		VARY,
		WARNING,

		// Validator
		ETAG,
		LAST_MODIFIED,

		// Authentication Challenges
		WWW_AUTHENTICATE,
		PROXY_AUTHENTICATE,

		// Response Context
		ACCEPT_RANGES,
		ALLOW,
		SERVER,

		// MIME
   		MIME_VERSION
	};

	constexpr const char* stringOf(Header header);
}
