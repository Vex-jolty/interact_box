#pragma once
#include "utils.hpp"
#include "file_util.hpp"
#include "shell_util.hpp"
#include "time_util.hpp"

namespace Utils {
	class CmdUtil {
		public:
			static void executeDosCommand(
				std::string command,
				std::string malwareDir,
				bool warnAboutUrlsInTerminal
			);

		private:
			static void _checkIfCommandIsValid(std::string command, std::string malwareDir);
			static bool _checkIfHasUrl(std::string command, std::vector<std::string>& out);
			static bool _checkIfHasBannedPath(
				std::string command,
				std::string path,
				ErrorCodes::ErrorCode code
			);
	};
} // namespace Utils
