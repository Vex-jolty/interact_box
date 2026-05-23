#include "http/http_request.hpp"

namespace Server::Http {
	using namespace std;
	HttpRequest::HttpRequest() {
		method = "UNKNOWN";
		route = "UNKNOWN";
		body = "";
	}

	void HttpRequest::parseContent(string requestContent) {
		method = getMethod(requestContent);
		route = getRoute(requestContent);
		query = getQueryFromRoute(route);
		body = getBody(requestContent);
	}

	smatch HttpRequest::getRegexMatch(regex pattern, const string& input) {
		smatch match;
		regex_search(input, match, pattern);
		if (!match.size())
			throw HttpStatus::BadRequest;
		return match;
	}

	Json::Value HttpRequest::getBody(const string& requestContent) {
		if (method == "GET")
			return Json::Value();
		if (requestContent.find("Content-Type: application/json") == string::npos)
			throw HttpStatus::UnprocessableEntity;
		regex pattern(R"(\{[\s\S]*\})");
		smatch match = getRegexMatch(pattern, requestContent);
		string result = match[0];
		return JsonHelper::parseJsonString(result);
	}

	string HttpRequest::getRoute(const string& requestContent) {
		regex pattern(R"(\s(/(?:(?:[\w\d\.\/\?=&]+)?/?)+)\sHTTP)");
		smatch match = getRegexMatch(pattern, requestContent);
		string route = match[1];
		return route;
	}

	map<string, string> HttpRequest::getQueryFromRoute(const string& route) {
		map<string, string> queryResult;
		if (route.find("=") == string::npos)
			return queryResult;
		regex queryPattern(R"(\??(\w+)=([\w\d]+))");
		smatch queryMatch = getRegexMatch(queryPattern, route);
		// Starting at 1 to skip question mark, and incrementing by 2 to get key-value pairs
		for (int i = 1; i < queryMatch.size(); i += 2) {
			queryResult[queryMatch[i]] = queryMatch[i + 1];
		}
		return queryResult;
	}

	string HttpRequest::getMethod(const string& requestContent) {
		string delimiter = " ";
		size_t pos = requestContent.find(delimiter);
		return requestContent.substr(0, pos);
	}
} // namespace Server::Http