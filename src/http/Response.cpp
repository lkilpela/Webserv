#include "Response.hpp"
#include <sstream>

std::string HttpResponse::toString() const {
    std::ostringstream response;
    response << version << " " << statusCode << " " << statusMessage << "\r\n";
    for (const auto& header : headers) {
        response << header.first << ": " << header.second << "\r\n";
    }
    response << "\r\n" << body;
    return response.str();
}

bool HttpResponse::validate() const {
    // Check for valid version
    if (version != "HTTP/1.1" && version != "HTTP/1.0") {
        return false;
    }

    // Check for valid status code
    if (statusCode < 100 || statusCode > 599) {
        return false;
    }

    // Check for required headers
    if (headers.find("Content-Length") == headers.end()) {
        return false;
    }

    return true;
}