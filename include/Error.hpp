#pragma once

#include <string>
#include <system_error>
#include <exception>
#include <iostream>

class WSException {
public:
    WSException(std::errc err)
    : errorCode(std::make_error_code(err)) {}
    WSException(std::error_code errorCode)
    : errorCode(errorCode) {}
    virtual ~WSException() = default;

    std::error_code code() const noexcept { return errorCode; }
private:
    std::error_code errorCode;
};

// Default ConfigError exception
class CongigError : public WSException {
public:
    CongigError(std::errc err = std::errc::invalid_argument)
    : WSException(err) {}
};

class NetworkError : public WSException {
public:
    NetworkError(std::errc err = std::errc::address_in_use)
    : WSException(err) {}
};

class RequestError : public WSException {
public:
    RequestError(std::errc err = std::errc::invalid_argument)
    : WSException(err) {}
};

class ResponseError : public WSException {
public:
    ResponseError(std::errc err = std::errc::invalid_argument)
    : WSException(err) {}
};

class RuntimeError : public WSException {
public:
    RuntimeError(std::errc err = std::errc::operation_not_permitted)
    : WSException(err) {}
};

class FileSystemError : public WSException {
public:
    FileSystemError(std::errc err = std::errc::no_such_file_or_directory)
    : WSException(err) {}
};


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