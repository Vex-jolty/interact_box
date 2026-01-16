#include "http/http_request.hpp"

namespace Server::Http {
	using namespace std;
	HttpRequest::HttpRequest(string requestContent) {
		method = getMethod(requestContent);
		route = getRoute(requestContent);
		body = getBody(requestContent);
	}

	smatch HttpRequest::getRegexMatch(regex pattern, string input) {
		smatch match;
		regex_search(input, match, pattern);
		if (!match.size())
			throw HttpStatus::BadRequest;
		return match;
	}

	Json::Value HttpRequest::getBody(string requestContent) {
		if (method == "GET")
			return Json::Value();
		if (requestContent.find("Content-Type: application/json") == string::npos)
			throw HttpStatus::UnprocessableEntity;
		regex pattern(R"(\{.*\})");
		smatch match = getRegexMatch(pattern, requestContent);
		string result = match[0];
		return JsonHelper::parseJsonString(result);
	}

	string HttpRequest::getRoute(string requestContent) {
		regex pattern(R"(\s(/(?:(?:[\w\d\.\/\?=&]+)?/?)+)\sHTTP)");
		smatch match = getRegexMatch(pattern, requestContent);
		string route = match[1];

		// Parse query
		if (route.find("=") != string::npos) {
			regex queryPattern(R"(\??(\w+)=([\w\d]+))");
			smatch queryMatch = getRegexMatch(queryPattern, route);
			// Starting at 1 to skip question mark, and incrementing by 2 to get key-value pairs
			for (int i = 1; i < queryMatch.size(); i + 2) {
				query[queryMatch[i]] = queryMatch[i + 1];
			}
		}
		return route;
	}

	string HttpRequest::getMethod(string requestContent) {
		string delimiter = " ";
		size_t pos = requestContent.find(delimiter);
		return requestContent.substr(0, pos);
	}
} // namespace Server::Http