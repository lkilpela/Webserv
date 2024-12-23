#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"

class CgiHandler {
public:
    HttpResponse executeCgi(const HttpRequest& request);
};

#endif // CGIHANDLER_HPP