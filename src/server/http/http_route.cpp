#include "http/http_route.hpp"

namespace Server::Http {
	using namespace std;
	HttpRoute::HttpRoute(
		string path,
		string method,
		function<void(HttpRequest* request, HttpResponse* response)> handler,
		optional<bool> condition
	) {
		_path = path;
		_method = method;
		_handler = handler;
		_condition = condition;
	}

	string HttpRoute::getPath() {
		return _path;
	}

	string HttpRoute::getMethod() {
		return _method;
	}
  void HttpRoute::executeHandler(HttpRequest* request, HttpResponse* response) {
		if (!_condition.value_or(true)) throw InteractBoxException(ErrorCodes::RouteIsDisabled, request->route);
		return _handler(request, response);
  }
}