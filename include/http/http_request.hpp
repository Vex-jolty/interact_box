#pragma once
#include <string>
#ifdef WIN32
	#include <winsock2.h>
	#include <windows.h>
	#include <ws2tcpip.h>
#endif
#include <regex>
#include "http_status_codes.h"
#include <map>
#include <ranges>
#include <iostream>
#include "json_helper.hpp"

namespace Server {
	namespace Http {
		class HttpRequest {
			public:
				HttpRequest();
				std::string method;
				Json::Value body;
				std::string route;
				std::map<std::string, std::string> query;
				void parseContent(std::string requestContent);

			private:
				std::string getMethod(const std::string& requestContent);
				Json::Value getBody(const std::string& requestContent);
				std::string getRoute(const std::string& requestContent);
				std::smatch getRegexMatch(std::regex pattern, const std::string& input);
				std::map<std::string, std::string> getQueryFromRoute(const std::string& route);
		};
	}; // namespace Http
} // namespace Server
