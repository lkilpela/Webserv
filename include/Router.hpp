#pragma once

#include <string>
#include <functional>
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "Config.hpp"

namespace http {
	// Forward declaration
	void handleGetRequest(const Location& loc, Request& request, Response& response);
	void handlePostRequest(const Location& loc, Request& request, Response& response);
	void handleDeleteRequest(const Location& loc, Request& request, Response& response);

	class Router {
		public:
			using Handler = std::function<void(const Location&, Request&, Response&)>;

			void get(Handler handler);
			void post(Handler handler);
			void del(Handler handler);
			void handle(Request& req, Response& res);

			void addLocations(const ServerConfig& serverConfig);


			void initRouter(const ServerConfig& serverConfig, Router& router) {
				// each server need one router
				router.addLocations(serverConfig);
				router.get(handleGetRequest);
				router.post(handlePostRequest);
				router.del(handleDeleteRequest);
			}

/* 		// Function to get the body of the response
		std::string getBody() const {
			if (_body) {
				return _body->toString();
			}
			return "";
		}

		// Function to set the response for string payloads
		void setStringResponse(Response& res, StatusCode statusCode, const std::string& body) {
			res.setStatusCode(statusCode);
			res.setBody(std::make_unique<utils::StringPayload>(0, body));
			res.build();
		}

		// Function to set the response for file payloads
		void setFileResponse(Response& res, StatusCode statusCode, const std::string& filePath) {
			res.setStatusCode(statusCode);
			res.setBody(std::make_unique<utils::FilePayload>(0, filePath));
			res.build();
		} */

		private:
			ServerConfig _serverConfig;
			std::unordered_map<std::string, Handler> _routes; //route -> method -> handler
			std::unordered_map<std::string, Location> _locationConfigs; // route -> location config

			const Location* findBestMatchingLocation(const std::string& url) const;
	};
}
