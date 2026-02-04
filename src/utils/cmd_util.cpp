#include "utils/cmd_util.hpp"

namespace Utils {
	using namespace std;

	void CmdUtil::executeTerminalCommand(string command, string malwareDir, bool warnAboutUrlsInTerminal) {
		/** cSpell:disable */
#ifdef WIN32
		if (!boost::iends_with(command, ";pause"))
			command += ";pause";

		malwareDir = StringHelper::toLowercase(malwareDir);
		_checkIfCommandIsValid(command, malwareDir);

	#if WINVER > _WIN32_WINNT_NT4
		wstring tempDir = L"C:\\WINDOWS\\TEMP";
	#else
		string tempDir = "C:\\WINDOWS\\TEMP";
	#endif

		size_t pos = command.find(';');
		while (pos != wstring::npos) {
			if (command[pos - 1] == '\\') {
				command.replace(pos - 1, 2, ";");
			} else {
				command.replace(pos, 1, "\r\n");
			}
			pos = command.find(';', pos + 1);
		}
	#if WINVER > _WIN32_WINNT_NT4
		wstring currentTime =
			StringHelper::stringToWideString(TimeUtil::getAndFormatCurrentTime("%Y-%m-%d-%H_%M"));
		wstring tempFile = L"CHAT-" + currentTime + L".BAT";
		wstring fullTempFilePath = tempDir + L"\\" + tempFile;
	#else
		string currentTime = TimeUtil::getAndFormatCurrentTime("%Y-%m-%d-%H_%M");
		string tempFile = "CHAT-" + currentTime + ".BAT";
		string fullTempFilePath = tempDir + "\\" + tempFile;
	#endif
		FileHelper::deleteFile(fullTempFilePath);
		HANDLE fileHandle = FileHelper::makeFile(fullTempFilePath, true);
		FileHelper::writeToFile(fileHandle, command);
		FileHelper::closeFile(fileHandle);

		if (warnAboutUrlsInTerminal) {
			vector<string> regexResults;
			bool hasUrl = _checkIfHasUrl(command, regexResults);
			if (hasUrl) {
	#if WINVER > _WIN32_WINNT_NT4
				int regexResultsSize = regexResults.size();
				string warningCommand =
					"@echo off\r\necho A command was sent containing the following URLs:\r\n";
				for (int i = 0; i < regexResultsSize; i++) {
					string item = regexResults[i];
					warningCommand += "echo " + item + "\r\n";
				}
				warningCommand += "set /P result=\"Do you wish to continue?(y/n)\"\r\n";
				warningCommand += "if /i NOT %result% == y goto end\r\n";

				warningCommand +=
					"start cmd /k call " + StringHelper::wideStringToString(tempFile) + "\r\n";
				warningCommand += ":end\r\n";
				warningCommand += "echo Done\r\n";

				wstring warningFileName = L"WARNING.BAT";
				wstring fullWarningFilePath = tempDir + L"\\" + warningFileName;
				if (FileHelper::checkIfFileExists(fullWarningFilePath)) {
					FileHelper::deleteFile(fullWarningFilePath);
				}
				HANDLE warningFileHandle = FileHelper::makeFile(fullWarningFilePath, true);
				FileHelper::writeToFile(warningFileHandle, warningCommand);
				FileHelper::closeFile(warningFileHandle);
				ShellUtil::openShell(warningFileName, L"open", tempDir);

	#else
				int regexResultsSize = regexResults.size();
				string warningCommand = "@echo off\r\necho A command was sent containing ";
				warningCommand +=
					regexResultsSize == 1 ? "1 URL:\r\n" : to_string(regexResultsSize) + " URLs:\r\n";
				for (int i = 0; i < regexResultsSize; i++) {
					string item = regexResults[i];
					warningCommand += "echo " + item + "\r\n";
				}
				warningCommand += "choice /CYN \"Do you wish to continue?\"\r\n";
				warningCommand += "if errorlevel 2 goto end\r\n";
				warningCommand += "start command.com /k " + tempFile + "\r\n";
				warningCommand += ":end\r\n";
				warningCommand += "echo Done\r\n";

				string warningFileName = "WARNING.BAT";
				string warningFilePath = tempDir + "\\" + warningFileName;
				FileHelper::deleteFile(warningFilePath);
				HANDLE warningFileHandle = FileHelper::makeFile(warningFilePath, true);
				FileHelper::writeToFile(warningFileHandle, warningCommand);
				FileHelper::closeFile(warningFileHandle);
				ShellUtil::openShell("COMMAND.COM", "open", nullopt, "/K " + warningFilePath);

	#endif
				ProcessHelper::setToForeground(warningFileName);
				return;
			}
		}

	#if WINVER > _WIN32_WINNT_NT4
		ShellUtil::openShell(tempFile, L"open", tempDir);
	#else
		ShellUtil::openShell("COMMAND.COM", "open", nullopt, "/K " + fullTempFilePath);
	#endif
		ProcessHelper::setToForeground(tempFile);
#else
		string tempDir = "/tmp";
		string currentTime = TimeUtil::getAndFormatCurrentTime("%Y-%m-%d-%H_%M");
		string tempFile = "chat-" + currentTime + ".sh";
		string fullTempFilePath = tempDir + "/" + tempFile;

		malwareDir = StringHelper::toLowercase(malwareDir);
		_checkIfCommandIsValid(command, malwareDir);
		system(fullTempFilePath.c_str());
#endif
	}

	void CmdUtil::_checkIfCommandIsValid(string command, string malwareDir) {
		map<string, ErrorCodes::ErrorCode> invalidCommandsAndErrors = {
#ifdef WIN32
			{"deltree", ErrorCodes::DelTreeCommand},
			{"del", ErrorCodes::DelCommand},
			{"erase", ErrorCodes::EraseCommand},
#else
			{"rm", ErrorCodes::DelCommand},
#endif
			{"trivia_game", ErrorCodes::TriviaGameCommand}
		};
		for (auto& [key, val] : invalidCommandsAndErrors) {
			if (boost::istarts_with(command, key) || boost::istarts_with(command, "@echo off;" + key)) {
				throw InteractBoxException(val);
			}
		}

		_checkIfHasBannedPath(command, malwareDir, ErrorCodes::MalwareCommand);
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
		_checkIfHasBannedPath(
			command, FileHelper::getWorkingDirectoryAsString(), ErrorCodes::IllegalDirectoryManipulation
		);
#else
		_checkIfHasBannedPath(
			command, FileHelper::getWorkingDirectory(), ErrorCodes::IllegalDirectoryManipulation
		);
#endif
	}

	bool CmdUtil::_checkIfHasUrl(string command, vector<string>& out) {
		regex urlPattern(
			R"((?:http|https|ftp)://(?:[\w-]+(?:\.[\w-]+)+)(?:[\w.,@?^=%&:/~+#-]*[\w@?^=%&/~+#-])?)"
		);
		smatch matches;
		for (sregex_iterator iter = sregex_iterator(command.begin(), command.end(), urlPattern);
				 iter != sregex_iterator(); iter++) {
			smatch match;
			match = *iter;
			out.push_back(match.str());
		}
		if (out.size() == 0)
			return false;
		return true;
	}

	bool CmdUtil::_checkIfHasBannedPath(string command, string path, ErrorCodes::ErrorCode code) {
		if (boost::icontains(command, path))
			throw InteractBoxException(code);

		// Check for partial path
		vector<string> splitPath = StringHelper::splitString(path, "\\");
		string pathName = splitPath[splitPath.size() - 1];
		if (boost::icontains(command, ("\\" + pathName)))
			throw InteractBoxException(code);

		// Check for attempt to get to path via variables
		regex pattern(R"(set\s*(?:\w+|\d+)+\s*=\s*(\"?(?:\w+:?\\?\s*)+\"?))");
		smatch match;
		regex_search(command, match, pattern);
		if (match.size() == 0)
			return false;

		vector<string> toJoin;

		for (int i = 1; i < match.size(); i++) {
			toJoin.push_back(match[i]);
		}
		string joinedString = boost::join(toJoin, "");
		if (boost::icontains(joinedString, path))
			throw InteractBoxException(code);

		return false;
	}
} // namespace Utils