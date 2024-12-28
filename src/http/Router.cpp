#include "Router.hpp"

namespace http {
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
		if (request.getStatus() == Request::Status::BAD_REQUEST) {
			response
				.setStatusCode(StatusCode::BAD_REQUEST_400)
				.setBody(nullptr)
				.build();
			return;
		}

		const std::string route = request.getUrl().path;
		auto it = _routes.find(route);

		// Matched a route
		if (it != _routes.end()) {
			const auto handlerByMethod = it->second;
			const auto handlerByMethodIt = handlerByMethod.find(request.getMethod());

			// The matched route does not support the requested http method, return http status 405
			if (handlerByMethodIt == handlerByMethod.end()) {
				response
					.setStatusCode(StatusCode::METHOD_NOT_ALLOWED_405)
					.setBody(nullptr)
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
					.setBody(nullptr)
					.build();
			}

			return;
		}

		// No matching route found, return http status 404
		response
			.setStatusCode(StatusCode::NOT_FOUND_404)
			.setBody(nullptr)
			.build();
	}
}
