#pragma once

#include "http/http_route.hpp"
#include "utils/file_util.hpp"
#include "utils/logging_util.hpp"
#include "utils/config_util.hpp"
#include "utils/resolution_util.hpp"
#include "utils/crash_util.hpp"
#include "utils/reboot_util.hpp"
#include "server/routes/route_handler_auxiliaries.hpp"

namespace Server::Routes {
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
	std::wstring processThemeCommand(
		std::shared_ptr<Utils::FileUtil> fileUtil,
		std::shared_ptr<Utils::LoggingUtil> loggingUtil
	);
	std::wstring processSoundCommand(std::shared_ptr<Utils::FileUtil> fileUtil);
	std::wstring processMalwareCommand(
		std::shared_ptr<Utils::FileUtil> fileUtil,
		std::shared_ptr<Utils::LoggingUtil> loggingUtil
	);
#else
	std::string processThemeCommand(
		std::shared_ptr<Utils::FileUtil> fileUtil,
		std::shared_ptr<Utils::LoggingUtil> loggingUtil
	);
	std::string processSoundCommand(std::shared_ptr<Utils::FileUtil> fileUtil);
	std::string processMalwareCommand(
		std::shared_ptr<Utils::FileUtil> fileUtil,
		std::shared_ptr<Utils::LoggingUtil> loggingUtil
	);
#endif

	class RouteHandler {
		public:
			RouteHandler(
				Utils::ConfigUtil& configUtil,
				std::shared_ptr<Utils::FileUtil> fileUtil,
				std::shared_ptr<Utils::LoggingUtil> loggingUtil,
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
				std::wstring msgBoxProcessName,
#elif defined(__linux__)
				std::string msgBoxProcessName
#else
				std::string msgBoxProcessName,
#endif
#ifdef WIN32
				pthread_mutex_t* themeMutex

			)
					: _configUtil(configUtil), _fileUtil(fileUtil), _loggingUtil(loggingUtil),
						_msgBoxProcessName(msgBoxProcessName), _themeMutex(themeMutex) {
				_setupRoutes();
			}
#else
			)
					: _configUtil(configUtil), _fileUtil(fileUtil), _loggingUtil(loggingUtil),
						_msgBoxProcessName(msgBoxProcessName) {
				_setupRoutes();
			}
#endif
			std::vector<Http::HttpRoute> getRoutes();

		private:
			Utils::ConfigUtil& _configUtil;
			std::shared_ptr<Utils::FileUtil> _fileUtil;
			std::shared_ptr<Utils::LoggingUtil> _loggingUtil;
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
			std::wstring _msgBoxProcessName;
#else
			std::string _msgBoxProcessName;
#endif
			std::vector<Http::HttpRoute> _routes;
#ifdef WIN32
			pthread_mutex_t* _themeMutex;
#endif

			void _setupRoutes();
	};
} // namespace Server::Routes
