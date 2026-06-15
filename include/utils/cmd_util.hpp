#pragma once
#include "utils.hpp"
#include "file_util.hpp"
#include "shell_util.hpp"
#include "time_util.hpp"

namespace Utils {
	class CmdUtil {
		public:
			static void executeTerminalCommand(
				std::string command,
				std::string malwareDir,
				bool warnAboutUrlsInTerminal
			);

		private:
			static void _checkIfCommandIsValid(const std::string& command, const std::string& malwareDir);
			static bool _checkIfHasUrl(const std::string& command, std::vector<std::string>& out);
			static bool _checkIfHasBannedPath(
				const std::string& command,
				const std::string& path,
				ErrorCodes::ErrorCode code
			);
	};
} // namespace Utils
