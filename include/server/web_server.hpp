#pragma once
#ifdef WIN32
	#ifndef _WINSOCK2API_
		#include <winsock2.h>
	#endif
#endif
#ifndef _GLIBCXX_STRING
	#include <string>
#endif
#ifndef _GLIBCXX_IOSTREAM
	#include <iostream>
#endif
#include "http/http_response.hpp"
#ifndef _GLIBCXX_VECTOR
	#include <vector>
#endif
#include "../utils/file_util.hpp"
#include "../utils/logging_util.hpp"
#include "http/http_request.hpp"
#include "http/http_route.hpp"
#include "http/http_response.hpp"
#include "errors/error_handler.hpp"
#include "errors/interact_box_exceptions.hpp"
#ifdef WIN32
	#ifndef _WS2TCPIP_H_
		#include <ws2tcpip.h>
	#endif
#endif
#include <atomic>

namespace Server {
	class WebServer {
		public:
			WebServer(
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
				std::wstring host,
#else
				std::string host,
#endif
				int port,
				std::shared_ptr<Utils::FileUtil> fileUtil,
				std::shared_ptr<Utils::LoggingUtil> loggingUtil,
				std::unique_ptr<Errors::ErrorHandler> errorHandler
			);
			void start();
			void respondWith(Http::HttpResponse response);
			void addRoute(Http::HttpRoute route);
			void addRoutes(std::vector<Http::HttpRoute> routes);
			void serverAbort();
			std::atomic<bool> abortNow;

		private:
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
			std::wstring _serverHost;
#else
			std::string _serverHost;
#endif
			int _port;
			SOCKET _listeningSocket;
			SOCKET _clientSocket;
			std::shared_ptr<Utils::FileUtil> _fileUtil;
			std::shared_ptr<Utils::LoggingUtil> _loggingUtil;
			std::vector<Http::HttpRoute> _routes;
			std::unique_ptr<Errors::ErrorHandler> _errorHandler;

			static void* handleRequest(void* arg);
			void sendHttpResponse(const std::string& response);
			std::string getClientIpAddress();
	};

	struct WebServerAndRequest {
			WebServer* server;
			Http::HttpRequest* request;
	};
} // namespace Server
