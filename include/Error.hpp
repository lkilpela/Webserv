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
class ConfigError : public WSException {
public:
    ConfigError(std::errc err = std::errc::invalid_argument)
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

/* std::errc - Member constants
Name	Equivalent POSIX Error
address_family_not_supported	EAFNOSUPPORT
address_in_use	EADDRINUSE
address_not_available	EADDRNOTAVAIL
already_connected	EISCONN
argument_list_too_long	E2BIG
argument_out_of_domain	EDOM
bad_address	EFAULT
bad_file_descriptor	EBADF
bad_message	EBADMSG
broken_pipe	EPIPE
connection_aborted	ECONNABORTED
connection_already_in_progress	EALREADY
connection_refused	ECONNREFUSED
connection_reset	ECONNRESET
cross_device_link	EXDEV
destination_address_required	EDESTADDRREQ
device_or_resource_busy	EBUSY
directory_not_empty	ENOTEMPTY
executable_format_error	ENOEXEC
file_exists	EEXIST
file_too_large	EFBIG
filename_too_long	ENAMETOOLONG
function_not_supported	ENOSYS
host_unreachable	EHOSTUNREACH
identifier_removed	EIDRM
illegal_byte_sequence	EILSEQ
inappropriate_io_control_operation	ENOTTY
interrupted	EINTR
invalid_argument	EINVAL
invalid_seek	ESPIPE
io_error	EIO
is_a_directory	EISDIR
message_size	EMSGSIZE
network_down	ENETDOWN
network_reset	ENETRESET
network_unreachable	ENETUNREACH
no_buffer_space	ENOBUFS
no_child_process	ECHILD
no_link	ENOLINK
no_lock_available	ENOLCK
no_message_available (deprecated)	ENODATA
no_message	ENOMSG
no_protocol_option	ENOPROTOOPT
no_space_on_device	ENOSPC
no_stream_resources (deprecated)	ENOSR
no_such_device_or_address	ENXIO
no_such_device	ENODEV
no_such_file_or_directory	ENOENT
no_such_process	ESRCH
not_a_directory	ENOTDIR
not_a_socket	ENOTSOCK
not_a_stream (deprecated)	ENOSTR
not_connected	ENOTCONN
not_enough_memory	ENOMEM
not_supported	ENOTSUP
operation_canceled	ECANCELED
operation_in_progress	EINPROGRESS
operation_not_permitted	EPERM
operation_not_supported	EOPNOTSUPP
operation_would_block	EWOULDBLOCK
owner_dead	EOWNERDEAD
permission_denied	EACCES
protocol_error	EPROTO
protocol_not_supported	EPROTONOSUPPORT
read_only_file_system	EROFS
resource_deadlock_would_occur	EDEADLK
resource_unavailable_try_again	EAGAIN
result_out_of_range	ERANGE
state_not_recoverable	ENOTRECOVERABLE
stream_timeout (deprecated)	ETIME
text_file_busy	ETXTBSY
timed_out	ETIMEDOUT
too_many_files_open_in_system	ENFILE
too_many_files_open	EMFILE
too_many_links	EMLINK
too_many_symbolic_link_levels	ELOOP
value_too_large	EOVERFLOW
wrong_protocol_type	EPROTOTYPE


*/