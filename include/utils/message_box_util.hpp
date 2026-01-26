#pragma once
#include "utils.hpp"
#include "shell_util.hpp"

namespace Utils {
	class MessageBoxUtil {
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
		public:
			static void createBox(
				std::wstring title,
				std::wstring content,
				std::wstring type,
				std::wstring buttons
			);

		private:
			static std::wstring getParsedString(std::wstring input);
#else
		public:
			static void createBox(
				std::string title,
				std::string content,
				std::string type,
				std::string buttons
			);

		private:
			static std::string getParsedString(std::string input);
#endif
	};
} // namespace Utils
