#pragma once
#include "utils.hpp"
#include "general_definitions.hpp"

namespace Utils {
	class ShellUtil {
	public:
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
	};
} // namespace Utils
