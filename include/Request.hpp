#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class HttpRequest {
public:
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

    static HttpRequest parse(const std::string& rawRequest);
    bool validate() const;
};

#endif // HTTPREQUEST_HPP