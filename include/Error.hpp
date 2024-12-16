#pragma once

#include <string>
#include <iostream>
#include <cerrno> 

class WSException {
public:
    WSException(int err)
    : errorCode(err, std::system_category()) {}
    virtual ~WSException() = default;

    std::error_code code() const noexcept { return errorCode; }
private:
    std::error_code errorCode;
};

// Default ConfigError exception
class ConfigError : public WSException {
public:
    ConfigError(int err, const std::string& msg)
    : WSException(err), message(msg) {}

    const char* what() const noexcept { return message.c_str(); }
private:
    std::string message;
};

class NetworkError : public WSException {
public:
    NetworkError(int err)
    : WSException(err) {}
};

class RequestError : public WSException {
public:
    RequestError(int err)
    : WSException(err) {}
};

class ResponseError : public WSException {
public:
    ResponseError(int err)
    : WSException(err) {}
};

class RuntimeError : public WSException {
public:
    RuntimeError(int err)
    : WSException(err) {}
};

class FileSystemError : public WSException {
public:
    FileSystemError(int err)
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