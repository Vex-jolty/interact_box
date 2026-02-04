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
	HttpResponse::HttpResponse() { setResponse(nullopt, "", 0); }
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
		if (!_headers.count("content-length")) {
			_headers["content-length"] = to_string(_body.size());
		}
		_statusCode = statusCode.value_or(HttpStatus::OK);
	}
	bool HttpResponse::isUnset() { return _body.empty() && _statusCode < 100; }
	string HttpResponse::toString() {
		const string statusCodeString = to_string(_statusCode);
		const string fullStatusString = statusCodeString + " " + HttpStatus::reasonPhrase(_statusCode);
		const string contentTypeString = "\r\nContent-Type: " + _headers["content-type"];
		const string contentLengthString = "\r\nContent-Length: " + _headers["content-length"];
		const string bodyString = "\r\n\r\n" + _body;
		const string result = "HTTP/1.1 " + fullStatusString + contentTypeString + contentLengthString + bodyString;
		return result;
	}
} // namespace Server::Http