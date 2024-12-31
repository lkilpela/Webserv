#include "Router.hpp"
#include "utils.hpp"

using namespace http;

Router::Router(ServerConfig config) : _config(config) {}

void Router::get(const std::string& route, Handler handler) {
	_routes[route]["GET"] = handler;
}

void Router::post(const std::string& route, Handler handler) {
	_routes[route]["POST"] = handler;
}

void Router::del(const std::string& route, Handler handler) {
	_routes[route]["DELETE"] = handler;
}

/**
 * Handle client request based on the registed `Handler` (callback).
 * This method call will modify response object in a way defined in callback
 *
 * @param request The request object must be COMPLETE or BAD_REQUEST
 * @param response
 */
void Router::handle(Request& request, Response& response) {
	const int socket = response.getClientSocket();

	if (request.getStatus() == Request::Status::BAD) {
		response
			.setStatusCode(StatusCode::BAD_REQUEST_400)
			.setHeader(Header::CONTENT_TYPE, getMimeType("html"))
			.setBody(std::make_unique<utils::FilePayload>(socket, _config.errorPages[400]))
			.build();
		return;
	}

	auto it = _routes.find(request.getUrl().path);

	// No matching route found, return http status 404
	if (it == _routes.end()) {
		response
			.setStatusCode(StatusCode::NOT_FOUND_404)
			.setHeader(Header::CONTENT_TYPE, getMimeType("html"))
			.setBody(std::make_unique<utils::FilePayload>(socket, _config.errorPages[404]))
			.build();
		return;
	}

	// Matched a route
	const auto handlerByMethod = it->second;
	const auto handlerByMethodIt = handlerByMethod.find(request.getMethod());

	// The matched route does not support the requested http method, return http status 405
	if (handlerByMethodIt == handlerByMethod.end()) {
		response
			.setStatusCode(StatusCode::METHOD_NOT_ALLOWED_405)
			.setHeader(Header::CONTENT_TYPE, getMimeType("html"))
			.setBody(std::make_unique<utils::FilePayload>(socket, _config.errorPages[405]))
			.build();
		return;
	}

	// Execute the handler, return http status 500 if an exception occurs
	try {
		const auto handler = handlerByMethodIt->second;
		handler(request, response);
	} catch(const std::exception& e) {
		response
			.clear()
			.setStatusCode(StatusCode::INTERNAL_SERVER_ERROR_500)
			.setHeader(Header::CONTENT_TYPE, getMimeType("html"))
			.setBody(std::make_unique<utils::FilePayload>(socket, _config.errorPages[500]))
			.build();
	}
}
