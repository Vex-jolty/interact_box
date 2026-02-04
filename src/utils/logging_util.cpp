#include "utils/logging_util.hpp"

namespace Utils {
	using namespace std;
	LoggingUtil::LoggingUtil(
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
		wstring fileName,
#else
		string fileName,
#endif
		LoggingLevel loggingLevel
	) {
		bool fileExists = FileHelper::checkIfFileExists(fileName);
#ifdef WIN32
		_fileHandle = FileHelper::makeFile(fileName, !fileExists);
#endif
		_fileName = fileName;
		_loggingLevel = loggingLevel;
		string initialMessage = "Current logging level is " + getLevelString(_loggingLevel) +
			". Launch with --loggingLevel <desired level> to alter this.";
		initialMessage += "\r\nPossible logging levels: debug, info, err, warn\r\n";
		FileHelper::writeToFile(_fileName, initialMessage);
	}

#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
	wstring LoggingUtil::getFileName() {
#else
	string LoggingUtil::getFileName() {
#endif
		return _fileName;
	}

	void LoggingUtil::log(string content, LoggingLevel level) {
		if (_loggingLevel > level)
			return;
		if (content.empty())
			return;
		string levelString = getLevelString(level);
		string dateTimeString = "[" + TimeUtil::getAndFormatCurrentTime("%Y-%m-%d %H:%M") + "]";
#ifdef WIN32
		boost::replace_all(content, "\n", "\r\n");
#endif
		if (!content.ends_with("\r\n"))
			content += "\r\n";
#ifdef WIN32
		FileHelper::writeToFile(_fileHandle, levelString + " " + dateTimeString + " " + content);
#else
		FileHelper::writeToFile(_fileName, levelString + " " + dateTimeString + " " + content);
#endif
	}

	void LoggingUtil::info(string content) { log(content, LoggingLevel::INFO); }

	void LoggingUtil::debug(string content) { log(content, LoggingLevel::DEBUG); }

	void LoggingUtil::warn(string content) { log(content, LoggingLevel::WARN); }

	void LoggingUtil::err(string content) { log(content, LoggingLevel::ERR); }

	void LoggingUtil::stopLogging() {
#ifdef WIN32
		FileHelper::closeFile(_fileHandle);
#endif
	}
	string LoggingUtil::getLevelString(LoggingLevel level) {
		switch (level) {
			case INFO:
				return "[INFO]";
			case DEBUG:
				return "[DEBUG]";
			case WARN:
				return "[WARN]";
			case ERR:
				return "[ERR]";
			default:
				return "[???]";
		}
	}
} // namespace Utils