#pragma once

#include <string>
#include <iostream>
#include <cerrno> 

class WSException {
public:
    WSException(int err, const std::string& msg = "", const std::string& file = "", const std::string& function = "", int line = 0)
    : errorCode(err, std::system_category())
    , message(msg.empty() ? std::system_category().message(err) : msg)
    , file(file)
    , function(function)
    , line(line) {}

    virtual ~WSException() = default;

    virtual std::error_code code() const noexcept { return errorCode; } // Return the error code from system
    virtual const char* what() const noexcept {
        fullMessage = message + " at " + file + ":" + std::to_string(line) + " in " + function;
        return fullMessage.c_str();
    }
private:
    std::error_code errorCode;
    std::string message;
    std::string file;
    std::string function;
    int line;
    mutable std::string fullMessage;
};

// Default ConfigError exception
class ConfigError : public WSException {
public:
    using WSException::WSException; // Inherit constructors from WSException
};

class NetworkError : public WSException {
public:
    using WSException::WSException;
};

class RouterError : public WSException {
public:
    using WSException::WSException;
};

class RequestError : public WSException {
public:
    using WSException::WSException;
};

class ResponseError : public WSException {
public:
    using WSException::WSException;
};

class RuntimeError : public WSException {
public:
    using WSException::WSException;
};

class FileSystemError : public WSException {
public:
    using WSException::WSException;
};

class FileNotFoundException : public std::runtime_error {
	public:
		explicit FileNotFoundException(const std::string& fileName) : std::runtime_error(fileName) {};
};

// Macro to throw exceptions with detailed information
#define THROW_CONFIG_ERROR(err, msg) throw ConfigError(err, msg, __FILE__, __FUNCTION__, __LINE__)

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

/* COMMON ERROR CODES:

- `recv`: 
  - `EAGAIN` or `EWOULDBLOCK`: The socket is marked non-blocking and the receive operation would block.
  - `EBADF`: The socket argument is not a valid file descriptor.
  - `ECONNRESET`: Connection reset by peer.
  - `EFAULT`: The receive buffer pointer is outside the process's address space.
  - `EINTR`: The receive was interrupted by delivery of a signal before any data was available.
  - `EINVAL`: Invalid argument passed.
  - `ENOTCONN`: The socket is not connected.
  - `ENOTSOCK`: The file descriptor does not refer to a socket.

- `socket`:
  - `EACCES`: Permission to create a socket of the specified type and/or protocol is denied.
  - `EAFNOSUPPORT`: The implementation does not support the specified address family.
  - `EINVAL`: Unknown protocol, or protocol family not available.
  - `EMFILE`: The per-process limit on the number of open file descriptors has been reached.
  - `ENFILE`: The system-wide limit on the total number of open files has been reached.
  - `ENOBUFS` or `ENOMEM`: Insufficient memory is available. The socket cannot be created until sufficient resources are freed.
  - `EPROTONOSUPPORT`: The protocol type or the specified protocol is not supported within this domain.

- `bind`:
  - `EACCES`: The address is protected, and the user is not the superuser.
  - `EADDRINUSE`: The given address is already in use.
  - `EBADF`: The socket argument is not a valid file descriptor.
  - `EINVAL`: The socket is already bound to an address.
  - `ENOTSOCK`: The file descriptor does not refer to a socket.
  - `EADDRNOTAVAIL`: The specified address is not available from the local machine.
  - `EFAULT`: The address is outside the user's accessible address space.

- `listen`:
  - `EADDRINUSE`: Another socket is already listening on the same port.
  - `EBADF`: The socket argument is not a valid file descriptor.
  - `ENOTSOCK`: The file descriptor does not refer to a socket.
  - `EOPNOTSUPP`: The socket is not of a type that supports the listen operation.

*/