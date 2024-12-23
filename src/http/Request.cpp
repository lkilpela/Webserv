#include "Request.hpp"
#include <sstream>

HttpRequest HttpRequest::parse(const std::string& rawRequest) {
    HttpRequest request;
    std::istringstream stream(rawRequest);
    std::string line;

    // Parse request line
    std::getline(stream, line);
    std::istringstream requestLine(line);
    requestLine >> request.method >> request.path >> request.version;

    // Parse headers
    while (std::getline(stream, line) && line != "\r") {
        std::string headerName, headerValue;
        std::istringstream headerLine(line);
        std::getline(headerLine, headerName, ':');
        std::getline(headerLine, headerValue);
        request.headers[headerName] = headerValue;
    }

    // Parse body
    if (request.headers.find("Content-Length") != request.headers.end()) {
        std::getline(stream, line); // Skip the empty line
        std::getline(stream, request.body, '\0');
    }

    return request;
}

bool HttpRequest::validate() const {
    // Check for valid method
    const std::string validMethods[] = {"GET", "PUT", "DELETE"};
    if (std::find(std::begin(validMethods), std::end(validMethods), method) == std::end(validMethods)) {
        return false;
    }

    // Check for valid version
    if (version != "HTTP/1.1" && version != "HTTP/1.0") {
        return false;
    }

    // Check for required headers
    if (headers.find("Host") == headers.end()) {
        return false;
    }

    return true;
}