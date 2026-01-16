#pragma once
#ifndef _GLIBCXX_STRING
	#include <string>
#endif
#ifndef _WINSOCK2API_
	#include <winsock2.h>
#endif
#ifndef _WS2TCPIP_H_
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
				SOCKET _clientSocket;
				std::string getMethod(std::string requestContent);
				Json::Value getBody(std::string requestContent);
				std::string getRoute(std::string requestContent);
				std::smatch getRegexMatch(std::regex pattern, std::string input);
		};
	}; // namespace Http
} // namespace Server
