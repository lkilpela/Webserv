#pragma once

#include <string>
#include <functional>
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"

namespace http {
	class Router {
		public:
			using Handler = std::function<void(Request&, Response&)>;

			void get(const Location& config, Handler handler);
			void post(const Location& config, Handler handler);
			void del(const Location& config, Handler handler);
			void handle(Request& req, Response& res);


		private:
			std::unordered_map<std::string, std::unordered_map<std::string, Handler>> _routes; //route -> method -> handler
			std::unordered_map<std::string, Location> _locationConfigs; // route -> location config

			const Location* findBestMatchingLocation(const std::string& url) const;
	};
}
