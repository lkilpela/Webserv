#pragma once

#include <string>
#include <functional>
#include "Config.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"

class Router {
	public:
		using Handler = std::function<void (http::Request &, http::Response &)>;

		explicit Router(ServerConfig config);

		void get(const std::string &route, Handler handler);
		void post(const std::string &route, Handler handler);
		void del(const std::string &route, Handler handler);
		void handle(http::Request &request, http::Response &response);

	private:
		ServerConfig _config;
		std::unordered_map<std::string, std::unordered_map<std::string, Handler>> _routes;
};
