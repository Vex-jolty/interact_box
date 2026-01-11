#include "utils/shell_util.hpp"

namespace Utils {
	using namespace std;
	HINSTANCE ShellUtil::openShell(string toOpen, string verb, optional<string> directory, optional<string> parameters, INT nShowCmd) {
		LPCSTR lpDir = directory.has_value() ? directory.value().c_str() : NULL;
		LPCSTR lpParams = parameters.has_value() ? parameters.value().c_str() : NULL;
		HINSTANCE instance = ShellExecuteA(NULL, verb.c_str(), toOpen.c_str(), lpParams, lpDir, nShowCmd);
		if ((int)instance <= 32) {
			throw InteractBoxException(ErrorCodes::CannotCallFromShell, toOpen);
		}
		return instance;
	}

	HINSTANCE ShellUtil::openShell(wstring toOpen, wstring verb, optional<wstring> directory, optional<wstring> parameters, INT nShowCmd) {
		LPCWSTR lpDir = directory.has_value() ? directory.value().c_str() : NULL;
		LPCWSTR lpParams = parameters.has_value() ? parameters.value().c_str() : NULL;
		HINSTANCE instance = ShellExecuteW(NULL, verb.c_str(), toOpen.c_str(), lpParams, lpDir, nShowCmd);
		if ((int)instance <= 32) {
			throw InteractBoxException(ErrorCodes::CannotCallFromShell, StringHelper::wideStringToString(toOpen));
		}
		return instance;
	}
}
