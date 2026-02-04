#pragma once
#ifndef _GLIBCXX_MAP
	#include <map>
#endif
#ifndef _GLIBCXX_STRING
	#include <string>
#endif
#ifndef _GLIBCXX_OPTIONAL
	#include <optional>
#endif
#ifdef WIN32
	#include <winsock2.h>
#endif
#include <boost/algorithm/string.hpp>
#include "http_status_codes.h"
#include "json_helper.hpp"

namespace Server {
	namespace Http {
		class HttpResponse {
			public:
				HttpResponse(
					std::optional<std::map<std::string, std::string>> headers,
					Json::Value jsonBody,
					std::optional<int> statusCode
				);
				HttpResponse();
				void setResponse(
					std::optional<std::map<std::string, std::string>> headers,
					Json::Value jsonBody,
					std::optional<int> statusCode
				);
				bool isUnset();
				std::string toString();

			private:
				std::map<std::string, std::string> _headers;
				std::string _body;
				int _statusCode;
		};
	} // namespace Http

} // namespace Server
