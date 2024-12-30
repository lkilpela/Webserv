#pragma once

#include <string>
#include <functional>
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"

namespace http {
	class Router {
		public:
			using Handler = std::function<void(Request&, Response&)>;

			explicit Router(Config config);
			void get(const std::string& route, Handler handler);
			void post(const std::string& route, Handler handler);
			void del(const std::string& route, Handler handler);
			void handle(Request& req, Response& res);

		private:
			Config _config;
			std::unordered_map<std::string, std::unordered_map<std::string, Handler>> _routes;
	};
}
