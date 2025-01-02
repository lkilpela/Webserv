#include <vector>
#include <string>

struct Location {
    std::string path;
    std::vector<std::string> methods;
};

class Response {
public:
    Response& setStatusCode(int code) {
        _statusCode = code;
        return *this;
    }

    Response& setBody(const std::string& body) {
        _body = body;
        return *this;
    }

    void build() {
        // Simulate building the response
    }

    const std::string& getBody() const {
        return _body;
    }

private:
    int _statusCode;
    std::string _body;
};

class Request {
public:
    Request(const std::string& url, const std::string& method)
        : _url(url), _method(method) {}

    const std::string& getUrl() const { return _url; }
    const std::string& getMethod() const { return _method; }

private:
    std::string _url;
    std::string _method;
};

using Handler = std::function<void(Request&, Response&)>;

void Router::handle(Request& request, Response& response) {
    const std::string route = request.getUrl();
    auto it = _routes.find(route);

    // Matched a route
    if (it != _routes.end()) {
        const auto& handlerByMethod = it->second;
        const auto handlerByMethodIt = handlerByMethod.find(request.getMethod());

        // The matched route does not support the requested http method, return http status 405
        if (handlerByMethodIt != handlerByMethod.end()) {
            handlerByMethodIt->second(request, response);
        } else {
            response.setStatusCode(405).setBody("Method Not Allowed").build();
        }
    } else {
        response.setStatusCode(404).setBody("Not Found").build();
    }
}

int main() {
    Router router;

    Location location = {"/index.html", {"GET"}};
    router.addRoute("/index.html", location, [](Request& req, Response& res) {
        res.setBody("Hello world");
    });

    // Create a request to test the route
    Request request("/index.html", "GET");
    Response response;

    router.handle(request, response);

    // Output the response body to verify the handler was called
    std::cout << "Response body: " << response.getBody() << std::endl;

    return 0;
}