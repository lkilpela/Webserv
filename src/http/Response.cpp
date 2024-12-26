#include "Response.hpp"
#include <sstream>


/* example: HTTP/1.1 200 OK
 *          Content-Type: text/html
 *          Content-Length: 1234
 *          Connection: close
 *          <empty line>
 *          <html>...</html>
 */
std::string HttpResponse::toString() const {
    // ostringstream means output string stream
    // what is ostringstream? it is a class to operate on strings like the iostream classes to operate on files
    // why use ostringstream? because it is used to create strings from variables of any data type
    std::ostringstream response;
    response << version << " " << statusCode << " " << statusMessage << "\r\n";
    // iterate over headers and append them to response
    for (const auto& header : headers) {
        response << header.first << ": " << header.second << "\r\n";
    }
    // append empty line \r\n means a new line
    response << "\r\n" << body;
    // return response as string
    // why use str()? because it returns a string object with a copy of the current contents of the stream
    // if no str() is used, it will return a reference to the string object with a copy of the current contents of the stream
    // example to see the difference:
    // std::ostringstream response;
    // response << "Hello, World!";
    // std::cout << response.str() << std::endl; // Hello, World!
    // std::cout << response << std::endl; // 0x7fffbf3f3b60
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