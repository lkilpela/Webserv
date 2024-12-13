#pragma once

#include <string>
#include <system_error>
#include <iostream>

template <typename T>
class CustomException {
public:
    CustomException(const std::string& message, std::errc errorCode)
        : message(message), errorCode(errorCode) {}

    const std::string& what() const noexcept { return message; }
    std::errc code() const noexcept { return errorCode; }

private:
    std::string message;
    std::errc errorCode;
};


// Define specific error types
struct ConfigError {};  // Tag for configuration errors
struct NetworkError {}; // Tag for network errors

// Type aliases for specific exceptions
using ConfigException = CustomException<ConfigError>;
using NetworkException = CustomException<NetworkError>;


/* PRIMARY EXCEPTION CLASSES
1. ConfigException
    - Invalid configuration syntax
    - Missing required fields
    - Invalid file paths
2. NetworkException
    - Unable to bind to a port
    - Address already in use
    - Connection timeouts
3. RequestException
    - Invalid HTTP request format (example: missing HTTP version, invalid URL)
    - Unsupported HTTP methods (example: PUT)
    - Invalid request headers (example: missing Host, Content-Length, etc.)
4. ResponseException
    - Missing default error pages (example: 404 Not Found)
    - Invalid CGI execution results (example: invalid headers, invalid body)
    - Invalid HTTP response format (example: missing status code, missing headers)
    - Invalid response headers (example: missing Content-Length, invalid Content-Type)
    - Invalid response body (example: missing body for 200 OK)
4. FileSystemException
    - File not found
    - Permission denied
    - File upload errors
5. RuntimeException
    - Unexpected errors
    - Memory allocation failures
    - Unhandled exceptions
*/