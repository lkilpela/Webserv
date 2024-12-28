#ifndef CONSTANT_HPP
#define CONSTANT_HPP

#include <string>

const std::string DEFAULT_INDEX_PATH = "/Users/lumik/Webserv/config/default/index.html";
const std::string DEFAULT_ERROR_PATH = "/Users/lumik/Webserv/config/default/error/";
const std::string DEFAULT_PATH = "/Users/lumik/Webserv/config/default/";
const std::string DEFAULT_CONTENT_TYPE = "text/html";
const int BUFFER_SIZE = 1024;

const std::string HTTP_1_1 = "HTTP/1.1";

// HTTP Status Codes
const int HTTP_OK = 200;
const int HTTP_BAD_REQUEST = 400;
const int HTTP_NOT_FOUND = 404;
const int HTTP_METHOD_NOT_ALLOWED = 405;
const int HTTP_INTERNAL_SERVER_ERROR = 500;

// HTTP Status Messages
const std::string HTTP_OK_MSG = "OK";
const std::string HTTP_BAD_REQUEST_MSG = "Bad Request";
const std::string HTTP_NOT_FOUND_MSG = "Not Found";
const std::string HTTP_METHOD_NOT_ALLOWED_MSG = "Method Not Allowed";
const std::string HTTP_INTERNAL_SERVER_ERROR_MSG = "Internal Server Error";

#endif // CONSTANT_HPP