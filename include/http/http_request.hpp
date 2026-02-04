#pragma once
#ifndef _GLIBCXX_STRING
	#include <string>
#endif
#ifdef WIN32
	#include <winsock2.h>
#endif
#ifdef WIN32
	#include <ws2tcpip.h>
#endif
#include <regex>
#ifndef HTTPSTATUSCODES_CPP_H_
	#include "http_status_codes.h"
#endif
#ifndef _GLIBCXX_MAP
	#include <map>
#endif
#include <ranges>
#ifndef _GLIBCXX_IOSTREAM
	#include <iostream>
#endif
#include "json_helper.hpp"

namespace Server {
	namespace Http {
		class HttpRequest {
			public:
				HttpRequest(std::string requestContent);
				std::string method;
				Json::Value body;
				std::string route;
				std::map<std::string, std::string> query;

			private:
				std::string getMethod(const std::string& requestContent);
				Json::Value getBody(const std::string& requestContent);
				std::string getRoute(const std::string& requestContent);
				std::smatch getRegexMatch(std::regex pattern, const std::string& input);
		};
	}; // namespace Http
} // namespace Server
