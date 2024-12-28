#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>

class HttpResponse {
public:
    std::string version;
    int statusCode;
    std::string statusMessage;
    std::map<std::string, std::string> headers;
    std::string body;

    std::string toString() const;
    bool validate() const;
};

#endif // HTTPRESPONSE_HPP