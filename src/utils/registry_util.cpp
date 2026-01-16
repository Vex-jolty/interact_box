#include "utils/registry_util.hpp"

namespace Utils {
	using namespace std;
#if WINVER > _WIN32_WINNT_NT4
	bool RegistryUtil::setNewKeyValue(
		HKEY topRegKeyToOpen,
		wstring regKeyNameToOpen,
		optional<wstring> newKey,
		wstring newValue
	) {
		HKEY hKey;
		long result = RegOpenKeyEx(topRegKeyToOpen, regKeyNameToOpen.c_str(), 0, KEY_SET_VALUE, &hKey);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotOpenRegistryKey, regKeyNameToOpen);

		result = RegSetValueEx(
			hKey, newKey.has_value() ? (*newKey).c_str() : NULL, 0, REG_SZ, (BYTE *)newValue.c_str(),
			(newValue.size() + 1) * sizeof(wchar_t)
		);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(
				ErrorCodes::CannotSetRegistryKey, newKey.value_or(regKeyNameToOpen)
			);
		result = RegCloseKey(hKey);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotCloseRegistryKey, regKeyNameToOpen);
		return true;
	}

	wstring RegistryUtil::getKeyValue(HKEY topRegKeyToOpen, wstring regKeyNameToOpen, wstring key) {
		HKEY hKey;
		WCHAR szBuffer[512];
		DWORD dwBufferSize = sizeof(szBuffer);
		long result =
			RegOpenKeyEx(topRegKeyToOpen, regKeyNameToOpen.c_str(), 0, KEY_QUERY_VALUE, &hKey);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotOpenRegistryKey, regKeyNameToOpen);
		result = RegQueryValueEx(hKey, key.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotGetRegistryKey, key);
		result = RegCloseKey(hKey);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotCloseRegistryKey, regKeyNameToOpen);
		return (wchar_t *)szBuffer;
	}

	vector<wstring> RegistryUtil::getListOfKeys(
		HKEY topRegKeyToOpen,
		wstring regKeyNameToOpen,
		Utils::LoggingUtil *loggingUtil,
		int startIndex
	) {
		HKEY hKey;
		long result = RegOpenKeyEx(topRegKeyToOpen, regKeyNameToOpen.c_str(), 0, KEY_READ, &hKey);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotOpenRegistryKey, regKeyNameToOpen);

		vector<wstring> keys;
		wchar_t subKeyName[256];
		DWORD subKeyNameSize;
		DWORD index = (DWORD)startIndex;
		while (true) {
			subKeyNameSize = sizeof(subKeyName);
			LONG result = RegEnumKeyEx(hKey, index, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL);
			if (result != ERROR_SUCCESS) {
				loggingUtil->err("Error while enumerating key:" + to_string(result));
				break;
			}
			loggingUtil->debug("Sub key name: " + StringHelper::wideStringToString(subKeyName) + "\n");
			keys.push_back(subKeyName);
			index++;
		}
		result = RegCloseKey(hKey);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotCloseRegistryKey, regKeyNameToOpen);
		return keys;
	}

#else
	bool RegistryUtil::setNewKeyValue(
		HKEY topRegKeyToOpen,
		string regKeyNameToOpen,
		optional<string> newKey,
		string newValue
	) {
		HKEY hKey;
		long result = RegOpenKeyEx(topRegKeyToOpen, regKeyNameToOpen.c_str(), 0, KEY_SET_VALUE, &hKey);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotOpenRegistryKey, regKeyNameToOpen);

		result = RegSetValueEx(
			hKey, newKey.has_value() ? (*newKey).c_str() : NULL, 0, REG_SZ, (BYTE *)newValue.c_str(),
			(newValue.size() + 1) * sizeof(wchar_t)
		);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(
				ErrorCodes::CannotSetRegistryKey, newKey.value_or(regKeyNameToOpen)
			);
		result = RegCloseKey(hKey);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotCloseRegistryKey, regKeyNameToOpen);
		return true;
	}

	string RegistryUtil::getKeyValue(HKEY topRegKeyToOpen, string regKeyNameToOpen, string key) {
		HKEY hKey;
		WCHAR szBuffer[512];
		DWORD dwBufferSize = sizeof(szBuffer);
		long result =
			RegOpenKeyEx(topRegKeyToOpen, regKeyNameToOpen.c_str(), 0, KEY_QUERY_VALUE, &hKey);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotOpenRegistryKey, regKeyNameToOpen);
		result = RegQueryValueEx(hKey, key.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotGetRegistryKey, key);
		result = RegCloseKey(hKey);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotCloseRegistryKey, regKeyNameToOpen);
		return (char *)szBuffer;
	}

	vector<string> RegistryUtil::getListOfKeys(
		HKEY topRegKeyToOpen,
		string regKeyNameToOpen,
		Utils::LoggingUtil *loggingUtil,
		int startIndex
	) {
		HKEY hKey;
		long result = RegOpenKeyEx(topRegKeyToOpen, regKeyNameToOpen.c_str(), 0, KEY_READ, &hKey);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotOpenRegistryKey, regKeyNameToOpen);

		vector<string> keys;
		char subKeyName[256];
		DWORD subKeyNameSize;
		DWORD index = (DWORD)startIndex;
		while (true) {
			subKeyNameSize = sizeof(subKeyName);
			LONG result = RegEnumKeyExA(hKey, index, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL);
			if (result != ERROR_SUCCESS) {
				loggingUtil->log("ERROR: " + to_string(result) + "\n");
				break;
			}
			loggingUtil->log("Sub key name: " + (string)subKeyName + "\n");
			keys.push_back(subKeyName);
			index++;
		}
		result = RegCloseKey(hKey);
		if (result != ERROR_SUCCESS)
			throw InteractBoxException(ErrorCodes::CannotCloseRegistryKey, regKeyNameToOpen);
		return keys;
	}
#endif
} // namespace Utils