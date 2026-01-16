#include "server/web_server.hpp"

namespace Server {
	using namespace std;
	WebServer::WebServer(
#if WINVER > _WIN32_WINNT_NT4
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
	}

	void WebServer::sendHttpResponse(const string &response) {
		send(_clientSocket, response.c_str(), response.size(), 0);
	}

	string WebServer::getClientIpAddress() {
		sockaddr_in clientAddr;
		int clientAddrSize = sizeof(clientAddr);

		if (getpeername(_clientSocket, (sockaddr *)&clientAddr, &clientAddrSize) == 0) {
			return inet_ntoa(clientAddr.sin_addr);
		} else {
			return "UNKNOWN";
		}
	}

	void *WebServer::handleRequest(void *arg) {
		WebServerAndRequest *webServerAndRequest = static_cast<WebServerAndRequest *>(arg);
		Http::HttpRequest *request = webServerAndRequest->request;
		WebServer *server = webServerAndRequest->server;
		Http::HttpResponse response = Http::HttpResponse();
		try {
			// Match route to existing route list
			auto iter =
				find_if(server->_routes.begin(), server->_routes.end(), [&request](Http::HttpRoute &route) {
					return boost::algorithm::iequals(request->route, route.getPath()) &&
						boost::algorithm::iequals(request->method, route.getMethod());
				});
			// Throw 404 if not found
			if (iter == server->_routes.end())
				throw HttpStatus::NotFound;

			iter.base()->executeHandler(request, &response);
			server->_loggingUtil->info("Response: " + response.toString());
			if (request->route == "/abort") {
				webServerAndRequest->server->serverAbort();
			}
		} catch (string &e) {
			server->_errorHandler->handleError(e);
			response.setResponse(nullopt, e, HttpStatus::InternalServerError);
		} catch (string *e) {
			server->_errorHandler->handleError(e);
			response.setResponse(nullopt, *e, HttpStatus::InternalServerError);
		} catch (InteractBoxException &e) {
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
		} catch (InteractBoxException *e) {
			server->_errorHandler->handleError(e);
			HttpStatus::Code status;
			if (boost::icontains(e->what(), "route is disabled")) {
				status = HttpStatus::Forbidden;
			} else if (boost::icontains(e->what(), "unsupported feature")) {
				status = HttpStatus::NotImplemented;
			} else {
				status = HttpStatus::InternalServerError;
			}
			response.setResponse(nullopt, e->what(), status);
		} catch (Json::Exception &e) {
			server->_errorHandler->handleError(e);
			response.setResponse(nullopt, e.what(), HttpStatus::BadRequest);
		} catch (Json::Exception *e) {
			server->_errorHandler->handleError(e);
			response.setResponse(nullopt, e->what(), HttpStatus::BadRequest);
		} catch (exception &e) {
			server->_errorHandler->handleError(e);
			response.setResponse(nullopt, e.what(), HttpStatus::InternalServerError);
		} catch (exception *e) {
			server->_errorHandler->handleError(e);
			response.setResponse(nullopt, e->what(), HttpStatus::InternalServerError);
		} catch (HttpStatus::Code &c) {
			server->_errorHandler->handleError(c, request->route);
			response.setResponse(nullopt, HttpStatus::reasonPhrase(c), c);
		}
		server->respondWith(response);
		free(request);
		return nullptr;
	}

	void WebServer::start() {
		WSADATA wsaData;
		sockaddr_in serverAddr, clientAddr;
		int clientAddrSize = sizeof(clientAddr);
		int bufferLength = 1024;
		char buffer[bufferLength];

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			throw InteractBoxException(ErrorCodes::WinSockStartupFailed);
		}

		_listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (_listeningSocket == INVALID_SOCKET) {
			WSACleanup();
			throw InteractBoxException(ErrorCodes::SocketCreationFailed);
		}
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(_port);
#if WINVER > _WIN32_WINNT_NT4
		serverAddr.sin_addr.s_addr = inet_addr(StringHelper::wideStringToString(_serverHost).c_str());
#else
		serverAddr.sin_addr.s_addr = inet_addr(_serverHost.c_str());
#endif
		if (bind(_listeningSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
			closesocket(_listeningSocket);
			WSACleanup();
			throw InteractBoxException(ErrorCodes::CannotBindToSocket);
		}

		if (listen(_listeningSocket, SOMAXCONN) == SOCKET_ERROR) {
			closesocket(_listeningSocket);
			WSACleanup();
			throw InteractBoxException(ErrorCodes::CannotListenToSocket);
		}

		_loggingUtil->debug("Server is listening on port " + _port);

		do {
			_clientSocket = accept(_listeningSocket, (sockaddr *)&clientAddr, &clientAddrSize);
			if (_clientSocket == INVALID_SOCKET) {
				_loggingUtil->err("Invalid client socket");
				break;
			}
			int bytesReceived = recv(_clientSocket, buffer, bufferLength, 0);
			if (bytesReceived > 0) {
				// Read the data that's received
				buffer[bytesReceived] = '\0';
				string requestContent(buffer);
				string ipAddress = getClientIpAddress();
				_loggingUtil->info(requestContent);
				_loggingUtil->info("Client IP: " + ipAddress);
				pthread_t serverThread;
				Http::HttpRequest *request = new Http::HttpRequest(requestContent);
				WebServerAndRequest webServerAndRequest{this, request};
				// Running the logic in a thread to prevent blocking, but joining to allow requests to
				// execute in order
				pthread_create(
					&serverThread, NULL, &WebServer::handleRequest, (void *)&webServerAndRequest
				);
				pthread_join(serverThread, NULL);
			}
			closesocket(_clientSocket);
		} while (!abortNow.load());

		// Clean up
		closesocket(_listeningSocket);
		WSACleanup();
	}

	void WebServer::respondWith(Http::HttpResponse response) {
		_loggingUtil->debug("Responded with: " + response.toString());
		sendHttpResponse(response.toString().c_str());
	}

	void WebServer::addRoute(Http::HttpRoute route) { _routes.push_back(route); }

	void WebServer::addRoutes(vector<Http::HttpRoute> routes) {
		for (auto &route : routes) {
			addRoute(route);
		}
	}
	void WebServer::serverAbort() {
		abortNow.store(true);
		if (_listeningSocket != INVALID_SOCKET) {
			closesocket(_listeningSocket);
			_listeningSocket = INVALID_SOCKET;
		}
	}
} // namespace Server