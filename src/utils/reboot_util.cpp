#include "utils/reboot_util.hpp"

namespace Utils {
	using namespace std;
	void RebootUtil::reboot() {
		#if WINVER > _WIN32_WINNT_NT4
		ShellUtil::openShell(L"shutdown", L"open", nullopt, L"/r /f /t 0");
		#else
		ShellUtil::openShell("RUNDLL32.EXE", "open", nullopt, "SHELL32.DLL,SHExitWindowsEx 6");
		#endif
	}
}