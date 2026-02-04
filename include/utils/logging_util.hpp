#pragma once
#include "utils.hpp"
#include "file_util.hpp"
#include "time_util.hpp"
#include "logging_level.hpp"

namespace Utils {
	class LoggingUtil {
		public:
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
			LoggingUtil(std::wstring fileName, LoggingLevel loggingLevel);
			std::wstring getFileName();
#else
			LoggingUtil(std::string fileName, LoggingLevel loggingLevel);
			std::string getFileName();
#endif
			void log(std::string content, LoggingLevel level = LoggingLevel::INFO);
			void info(std::string content);
			void debug(std::string content);
			void warn(std::string content);
			void err(std::string content);
			void stopLogging();

		private:
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
			std::wstring _fileName;
#else
			std::string _fileName;
#endif
			std::streambuf* _streamBuffer;
#ifdef WIN32
			HANDLE _fileHandle;
#endif
			LoggingLevel _loggingLevel;
			std::string getLevelString(LoggingLevel level);
	};
} // namespace Utils
