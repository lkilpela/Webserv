#pragma once

#include <string>
#include <functional>
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "Config.hpp"

namespace http {
	class Router {
		public:
			using Handler = std::function<void(const Location&, Request&, Response&)>;

			void get(Handler handler);
			void post(Handler handler);
			void del(Handler handler);
			void handle(Request& req, Response& res);

			void addLocations(const ServerConfig& serverConfig);
			//void handleGetRequest(Location loc, Request& request, Response& response);

		private:
			ServerConfig _serverConfig;
			std::unordered_map<std::string, Handler> _routes; //route -> method -> handler
			std::unordered_map<std::string, Location> _locationConfigs; // route -> location config

			const Location* findBestMatchingLocation(const std::string& url) const;
	};
}
