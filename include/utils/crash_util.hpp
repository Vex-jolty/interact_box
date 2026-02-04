#pragma once
#include "utils.hpp"
#ifdef WIN32
	#if WINVER <= _WIN32_WINNT_NT4
		#include "shell_util.hpp"
	#endif
	#include <winternl.h>

	#define MAX_U_SHORT_SIZE 65535
	#define OPTION_SHUTDOWN 6
#else
	#include <thread>
	#include <chrono>
#endif

namespace Utils {
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
	typedef NTSTATUS(NTAPI* TFNRtlAdjustPrivilege)(
		ULONG Privilege,
		BOOLEAN Enable,
		BOOLEAN CurrentThread,
		PBOOLEAN Enabled
	);

	typedef NTSTATUS(NTAPI* TFNNtRaiseHardError)(
		NTSTATUS ErrorStatus,
		ULONG NumberOfParameters,
		ULONG UnicodeStringParameterMask,
		PULONG_PTR* Parameters,
		ULONG ValidResponseOption,
		PULONG Response
	);
#endif
	class CrashUtil {
		public:
			static void crash();

		private:
			static void* runCrashThread(void* arg);
	};
} // namespace Utils