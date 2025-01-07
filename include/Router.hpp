#pragma once

#include <string>
#include <functional>
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "Config.hpp"
#include "http/Url.hpp"
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <fstream>

// Forward declaration
void handleGetRequest(const Location& loc, http::Request& request, http::Response& response);
void handlePostRequest(const Location& loc, http::Request& request, http::Response& response);
void handleDeleteRequest(const Location& loc, http::Request& request, http::Response& response);

class Router {
	public:
		using Handler = std::function<void(const Location&, http::Request&, http::Response&)>;

		Router(const ServerConfig& serverConfig) : _serverConfig(serverConfig) {
			addLocations(serverConfig);
		}

		void get(Handler handler);
		void post(Handler handler);
		void del(Handler handler);
		void handle(http::Request& req, http::Response& res);

		void addLocations(const ServerConfig& serverConfig);


/* 		void initRouter(const ServerConfig& serverConfig, Router& router) {
			// each server need one router
			//router.addLocations(serverConfig);
			// how to know which method to call? 
			// Answer: use the method name as the key in the map
			// and the value is the function to call
			// 
			router.get(handleGetRequest);
			router.post(handlePostRequest);
			router.del(handleDeleteRequest);
		} */

	private:
		ServerConfig _serverConfig;
		std::unordered_map<std::string, Handler> _routes; // method -> handler
		std::unordered_map<std::string, Location> _locationConfigs; // route -> location config

		const Location* findBestMatchingLocation(const std::string& url) const;
};
