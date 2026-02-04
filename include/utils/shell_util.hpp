#pragma once
#include "utils.hpp"
#ifdef __linux__
	#include <spawn.h>
#else
	#include "general_definitions.hpp"
#endif

namespace Utils {
	class ShellUtil {
		public:
#ifdef WIN32
			static HINSTANCE openShell(
				std::string toOpen,
				std::string verb = "open",
				std::optional<std::string> directory = std::nullopt,
				std::optional<std::string> parameters = std::nullopt,
				INT nShowCmd = SW_SHOW
			);
			static HINSTANCE openShell(
				std::wstring toOpen,
				std::wstring verb = L"open",
				std::optional<std::wstring> directory = std::nullopt,
				std::optional<std::wstring> parameters = std::nullopt,
				INT nShowCmd = SW_NORMAL
			);
#else
			static pid_t openShell(
				const std::string& path,
				std::optional<std::string> parameters = std::nullopt
			);
#endif
	};
} // namespace Utils
