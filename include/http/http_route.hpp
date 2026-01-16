#pragma once
#ifndef _GLIBCXX_STRING
	#include <string>
#endif
#include <optional>
#include "http_request.hpp"
#include "http_response.hpp"

namespace Server::Http {
	class HttpRoute {
		public:
			HttpRoute(
				std::string path,
				std::string method,
				std::function<void(HttpRequest *request, HttpResponse *response)> handler,
				std::optional<bool> condition = std::nullopt
			);
			std::string getPath();
			std::string getMethod();
			void executeHandler(HttpRequest *request, HttpResponse *response);

		private:
			std::string _path;
			std::string _method;
			std::function<void(HttpRequest *request, HttpResponse *response)> _handler;
			std::optional<bool> _condition;
	};
} // namespace Server::Http
