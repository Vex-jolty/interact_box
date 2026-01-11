#pragma once
#include "errors/errors.hpp"
#include "http_status_codes.h"
#include "string_helper.hpp"

namespace Errors {
	class ErrorHandler {
		public:
			ErrorHandler(
				std::shared_ptr<Utils::LoggingUtil> loggingUtil,
				#if WINVER > _WIN32_WINNT_NT4
				std::wstring messageBoxPath
				#else
				std::string messageBoxPath
				#endif
			);
			void handleError(HttpStatus::Code& c, std::string& route);
			#if WINVER > _WIN32_WINNT_NT4
			void handleError(std::wstring& e);
			void handleError(std::wstring* e);
			#endif
			void handleError(std::string& e);
			void handleError(std::string* e);
			void handleError(std::exception& e);
			void handleError(std::exception* e);
			void handleError(InteractBoxException& e);
			void handleError(InteractBoxException* e);

		private:
			std::shared_ptr<Utils::LoggingUtil> _loggingUtil;
			#if WINVER > _WIN32_WINNT_NT4
			std::wstring _messageBoxPath;
			void showErrorBox(std::wstring message);
			void showErrorBox(std::string message);
			std::wstring _convertMessage(std::string message);
			#else
			std::string _messageBoxPath;
			void showErrorBox(std::string message);
			#endif
	};
} // namespace Errors
