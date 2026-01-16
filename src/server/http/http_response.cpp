#include "http/http_response.hpp"

namespace Server::Http {
	using namespace std;
	HttpResponse::HttpResponse(
		optional<map<string, string>> headers,
		Json::Value jsonBody,
		optional<int> statusCode
	) {
		setResponse(headers, jsonBody, statusCode);
	}
	HttpResponse::HttpResponse() { setResponse(nullopt, "", 1); }
	void HttpResponse::setResponse(
		optional<map<string, string>> headers,
		Json::Value jsonBody,
		optional<int> statusCode
	) {
		_headers = headers.value_or(map<string, string>());
		// Defaulting to JSON content-type
		if (!_headers.count("content-type")) {
			_headers["content-type"] = "application/json";
		}
		_body = jsonBody.toStyledString();
		_statusCode = statusCode.value_or(HttpStatus::OK);
	}
	bool HttpResponse::isUnset() { return _body.empty() && _statusCode < 100; }
	string HttpResponse::toString() {
		string statusCodeString = to_string(_statusCode);
		string fullStatusString = statusCodeString + " " + HttpStatus::reasonPhrase(_statusCode);
		string contentTypeString = "\r\nContent-Type: " + _headers["content-type"];
		string bodyString = "\r\n\r\n" + _body;
		string result = "HTTP/1.1 " + fullStatusString + contentTypeString + bodyString;
		return result;
	}
} // namespace Server::Http