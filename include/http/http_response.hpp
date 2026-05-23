#pragma once
#include <map>
#include <string>
#include <optional>
#ifdef WIN32
	#include <winsock2.h>
	#include <windows.h>
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
				bool isOk();

			private:
				std::map<std::string, std::string> _headers;
				std::string _body;
				int _statusCode;
		};
	} // namespace Http

} // namespace Server
