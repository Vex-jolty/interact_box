#include "server/web_server.hpp"

namespace Server {
	using namespace std;
	WebServer::WebServer(
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
		wstring host,
#else
		string host,
#endif
		int port,
		shared_ptr<Utils::FileUtil> fileUtil,
		shared_ptr<Utils::LoggingUtil> loggingUtil,
		unique_ptr<Errors::ErrorHandler> errorHandler
	) {
		abortNow = false;
		_serverHost = host;
		_port = port;
		_fileUtil = fileUtil;
		_loggingUtil = loggingUtil;
		_errorHandler = move(errorHandler);
#ifdef _WIN32
		_socket = std::make_unique<WinSocketBackend>();
#else
		_socket = std::make_unique<LinuxSocketBackend>();
#endif
	}

	void* WebServer::handleRequest(void* arg) {
		WebServerAndRequest* ctx = static_cast<WebServerAndRequest*>(arg);
		Http::HttpRequest* request = ctx->request;
		WebServer* server = ctx->server;
		ctx->server->_loggingUtil->debug("Now handling request...");

		Http::HttpResponse response;

		try {
			auto it =
				std::find_if(server->_routes.begin(), server->_routes.end(), [&](Http::HttpRoute& r) {
					return boost::iequals(r.getPath(), request->route) &&
						boost::iequals(r.getMethod(), request->method);
				});

			if (it == server->_routes.end())
				throw HttpStatus::NotFound;
			server->_loggingUtil->debug("About to execute handler");
			it->executeHandler(request, &response);
			server->_loggingUtil->debug("Handler executed");
			if (request->route == "/abort")
				server->serverAbort();
		} catch (InteractBoxException& e) {
			server->_errorHandler->handleError(e);
			HttpStatus::Code status;
			if (boost::icontains(e.what(), "route is disabled")) {
				status = HttpStatus::Forbidden;
			} else if (boost::icontains(e.what(), "unsupported feature")) {
				status = HttpStatus::NotImplemented;
			} else {
				status = HttpStatus::InternalServerError;
			}
			response.setResponse(nullopt, e.what(), status);
		} catch (Json::Exception& e) {
			server->_errorHandler->handleError(e);
			response.setResponse(nullopt, e.what(), HttpStatus::BadRequest);
		} catch (exception& e) {
			server->_errorHandler->handleError(e);
			response.setResponse(nullopt, e.what(), HttpStatus::InternalServerError);
		} catch (HttpStatus::Code& c) {
			server->_errorHandler->handleError(c, request->route);
			response.setResponse(nullopt, HttpStatus::reasonPhrase(c), c);
		} catch (...) {
			response.setResponse(nullopt, "", HttpStatus::InternalServerError);
		}

		try {
			server->respondWith(ctx->clientFd, response);
		} catch (InteractBoxException& e) {
			server->_errorHandler->handleError(e);
		}
		delete request;
		return nullptr;
	}

	void WebServer::start() {
		if (!_socket->init()) {
			throw InteractBoxException(ErrorCodes::WinSockStartupFailed);
		}

		if (!_socket->bindAndListen(
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
					StringHelper::wideStringToString(_serverHost),
#else
					_serverHost,
#endif
					_port
				)) {
			_socket->shutdown();
			throw InteractBoxException(ErrorCodes::CannotBindToSocket);
		}

		_loggingUtil->info("Server listening on port " + std::to_string(_port));

		do {
			std::string clientIp;
			int clientFd = _socket->acceptClient(clientIp);
			if (clientFd < 0) {
				if (abortNow.load())
					break;
				continue;
			}

			std::string requestData;
			int received = _socket->recvData(clientFd, requestData);
			cout << requestData << "\n";

			if (received <= 0) {
				_socket->closeClient(clientFd);
				continue;
			}

			_loggingUtil->info("Client IP: " + clientIp);
			_loggingUtil->info(requestData);

			Http::HttpRequest* request = new Http::HttpRequest(requestData);
			cout << request->method << "\n" << request->route << "\n";

			WebServerAndRequest ctx;
			ctx.server = this;
			ctx.request = request;
			ctx.clientFd = clientFd;

			pthread_t worker;
			pthread_create(&worker, nullptr, &WebServer::handleRequest, &ctx);
			pthread_join(worker, nullptr);

			_socket->closeClient(clientFd);
		} while (!abortNow.load());

		_socket->shutdown();
	}

	void WebServer::respondWith(int clientFd, Http::HttpResponse response) {
		_loggingUtil->debug("Response:\n" + response.toString());
		_socket->sendData(clientFd, response.toString());
	}

	void WebServer::addRoute(Http::HttpRoute route) { _routes.push_back(route); }

	void WebServer::addRoutes(vector<Http::HttpRoute> routes) {
		for (auto& route : routes) {
			addRoute(route);
		}
	}
	void WebServer::serverAbort() {
		abortNow.store(true);
#if WIN32
		if (_listeningSocket != INVALID_SOCKET) {
			closesocket(_listeningSocket);
			_listeningSocket = INVALID_SOCKET;
		}
#endif
	}
} // namespace Server