#pragma once
#include "utils.hpp"
#include "logging_util.hpp"
namespace Utils {
	class RegistryUtil {
		public:
			#if WINVER > _WIN32_WINNT_NT4
			static bool setNewKeyValue(HKEY topRegKeyToOpen, std::wstring regKeyNameToOpen, std::optional<std::wstring> newKey, std::wstring newValue);
			static std::wstring getKeyValue(HKEY topRegKeyToOpen, std::wstring regKeyNameToOpen, std::wstring key);
			static std::vector<std::wstring> getListOfKeys(HKEY topRegKeyToOpen, std::wstring regKeyNameToOpen, Utils::LoggingUtil* loggingUtil, int startIndex = 0);
			#else
			static bool setNewKeyValue(HKEY topRegKeyToOpen, std::string regKeyNameToOpen, std::optional<std::string> newKey, std::string newValue);
			static std::string getKeyValue(HKEY topRegKeyToOpen, std::string regKeyNameToOpen, std::string key);
			static std::vector<std::string> getListOfKeys(HKEY topRegKeyToOpen, std::string regKeyNameToOpen, Utils::LoggingUtil* loggingUtil, int startIndex = 0);
			#endif
	};
} // namespace Utils
