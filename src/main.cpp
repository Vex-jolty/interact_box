#include "setup.hpp"

using namespace std;
using namespace Server;

#pragma comment(lib, "dbghelp.lib")

#if WINVER > _WIN32_WINNT_NT4
wstring getExceptionName(DWORD code) {
	switch (code) {
		case EXCEPTION_ACCESS_VIOLATION:
			return L"EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			return L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
		case EXCEPTION_BREAKPOINT:
			return L"EXCEPTION_BREAKPOINT";
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			return L"EXCEPTION_DATATYPE_MISALIGNMENT";
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			return L"EXCEPTION_FLT_DENORMAL_OPERAND";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			return L"EXCEPTION_FLT_DIVIDE_BY_ZERO";
		case EXCEPTION_FLT_INEXACT_RESULT:
			return L"EXCEPTION_FLT_INEXACT_RESULT";
		case EXCEPTION_FLT_INVALID_OPERATION:
			return L"EXCEPTION_FLT_INVALID_OPERATION";
		case EXCEPTION_FLT_OVERFLOW:
			return L"EXCEPTION_FLT_OVERFLOW";
		case EXCEPTION_FLT_STACK_CHECK:
			return L"EXCEPTION_FLT_STACK_CHECK";
		case EXCEPTION_FLT_UNDERFLOW:
			return L"EXCEPTION_FLT_UNDERFLOW";
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			return L"EXCEPTION_ILLEGAL_INSTRUCTION";
		case EXCEPTION_IN_PAGE_ERROR:
			return L"EXCEPTION_IN_PAGE_ERROR";
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			return L"EXCEPTION_INT_DIVIDE_BY_ZERO";
		case EXCEPTION_INT_OVERFLOW:
			return L"EXCEPTION_INT_OVERFLOW";
		case EXCEPTION_INVALID_DISPOSITION:
			return L"EXCEPTION_INVALID_DISPOSITION";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			return L"EXCEPTION_NONCONTINUABLE_EXCEPTION";
		case EXCEPTION_PRIV_INSTRUCTION:
			return L"EXCEPTION_PRIV_INSTRUCTION";
		case EXCEPTION_SINGLE_STEP:
			return L"EXCEPTION_SINGLE_STEP";
		case EXCEPTION_STACK_OVERFLOW:
			return L"EXCEPTION_STACK_OVERFLOW";
		default:
			return wstring();
	}
}

// Optional wide string to inform calling function of potential failure, or of file name
optional<wstring> createMinidump(EXCEPTION_POINTERS* exceptionPointers) {
	TCHAR szFileName[MAX_PATH];
	wstring localTime = StringHelper::stringToWideString(Utils::TimeUtil::getAndFormatCurrentTime("%Y-%m-%d-%H_%M"));
	wstring workingDir = FileHelper::getWorkingDirectory();
	wstring fileName = L"crashDump-" + localTime + L".dmp";

	HANDLE fileHandle = FileHelper::makeFile(workingDir + L"\\" + fileName);
	if (fileHandle == INVALID_HANDLE_VALUE) return nullopt;

	MINIDUMP_EXCEPTION_INFORMATION mdei;
	mdei.ThreadId = GetCurrentThreadId();
	mdei.ExceptionPointers = exceptionPointers;

	MiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		fileHandle,
		MiniDumpNormal,
		(exceptionPointers ? &mdei : nullptr),
		nullptr,
		nullptr
	);

	FileHelper::closeFile(fileHandle);
	return fileName;
}

LONG CALLBACK vectoredExceptionHandler(PEXCEPTION_POINTERS exceptionPointers) {
	wstring code = getExceptionName(exceptionPointers->ExceptionRecord->ExceptionCode);
	if (code.empty()) return EXCEPTION_EXECUTE_HANDLER;
	optional<wstring> miniDumpFile = createMinidump(exceptionPointers);
	DWORD lastError = GetLastError();
	wstring message = L"Interact Box has encountered an unhandled error.\nException code: " +
		code +
		L"\n" +
		L"Error code: " +
		to_wstring(lastError) + L"\n";
	message +=
		(
			miniDumpFile.has_value() ?
			L"A minidump file has been created (" + miniDumpFile.value() + L")" :
			L"An attempt to create a minidump file failed"
		) + L"\n";
	message += L"This message should never be shown. Something seriously wrong occured. Please provide the dump file to the developer, with a description of what you were doing and what route was called, along with any relevant details.\n";
	message += L"Interact Box will now attempt to recover from this error.";
	MessageBox(
		NULL,
		message.c_str(),
		L"INTERACT BOX UNHANDLED ERROR",
		MB_ICONERROR
	);
	return EXCEPTION_EXECUTE_HANDLER;
}

#else

string getExceptionName(DWORD code) {
	switch (code) {
		case EXCEPTION_ACCESS_VIOLATION:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
		case EXCEPTION_BREAKPOINT:
			return "EXCEPTION_BREAKPOINT";
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			return "EXCEPTION_DATATYPE_MISALIGNMENT";
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			return "EXCEPTION_FLT_DENORMAL_OPERAND";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
		case EXCEPTION_FLT_INEXACT_RESULT:
			return "EXCEPTION_FLT_INEXACT_RESULT";
		case EXCEPTION_FLT_INVALID_OPERATION:
			return "EXCEPTION_FLT_INVALID_OPERATION";
		case EXCEPTION_FLT_OVERFLOW:
			return "EXCEPTION_FLT_OVERFLOW";
		case EXCEPTION_FLT_STACK_CHECK:
			return "EXCEPTION_FLT_STACK_CHECK";
		case EXCEPTION_FLT_UNDERFLOW:
			return "EXCEPTION_FLT_UNDERFLOW";
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			return "EXCEPTION_ILLEGAL_INSTRUCTION";
		case EXCEPTION_IN_PAGE_ERROR:
			return "EXCEPTION_IN_PAGE_ERROR";
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			return "EXCEPTION_INT_DIVIDE_BY_ZERO";
		case EXCEPTION_INT_OVERFLOW:
			return "EXCEPTION_INT_OVERFLOW";
		case EXCEPTION_INVALID_DISPOSITION:
			return "EXCEPTION_INVALID_DISPOSITION";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
		case EXCEPTION_PRIV_INSTRUCTION:
			return "EXCEPTION_PRIV_INSTRUCTION";
		case EXCEPTION_SINGLE_STEP:
			return "EXCEPTION_SINGLE_STEP";
		case EXCEPTION_STACK_OVERFLOW:
			return "EXCEPTION_STACK_OVERFLOW";
		default:
			return string();
	}
}

LONG WINAPI topLevelExceptionHandler(EXCEPTION_POINTERS* pExceptionInfo) {
	HANDLE hFile = CreateFile("crashdump.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
		exceptionInfo.ThreadId = GetCurrentThreadId();
		exceptionInfo.ExceptionPointers = pExceptionInfo;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &exceptionInfo, NULL, NULL);
		CloseHandle(hFile);
	}
	string code = getExceptionName(pExceptionInfo->ExceptionRecord->ExceptionCode);
	string errorMessage = "Interact Box has encountered a fatal error and will be shut down.\n";
	errorMessage += "Error code: " + to_string(GetLastError()) + "\n" + "Exception Code: " + code + "\n";
	errorMessage += "A minidump file has been created (crashdump.dmp)";
	MessageBox(NULL, errorMessage.c_str(), "INTERACT BOX FATAL ERROR", MB_ICONERROR);
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif
pthread_mutex_t themeMutex = PTHREAD_MUTEX_INITIALIZER;
// cSpell:enable

LoggingLevel setLoggingLevel(int argc, char* argv[]) {
	if (argc < 2 || argv[1] != "--loglevel") return LoggingLevel::DEBUG;
	if (boost::algorithm::istarts_with(argv[2], "err")) return LoggingLevel::ERR;
	else if (boost::algorithm::istarts_with(argv[2], "warn")) return LoggingLevel::WARN;
	else if (boost::algorithm::istarts_with(argv[2], "info")) return LoggingLevel::INFO;
	return LoggingLevel::DEBUG;
}

int main(int argc, char* argv[]) {
#if WINVER > _WIN32_WINNT_NT4
	PVOID handler = AddVectoredExceptionHandler(1, vectoredExceptionHandler);
	Utils::ConfigUtil configUtil(StringHelper::wideStringToString(FileHelper::getWindowsDirectory()) + "\\interact_box_config.json");
	wstring host = configUtil.getHost();
#else
	SetUnhandledExceptionFilter(topLevelExceptionHandler);

	Utils::ConfigUtil configUtil(FileHelper::getWindowsDirectory() + "\\interact_box_config.json");

	string host = configUtil.getHost();
#endif
	int port = configUtil.getPort();

	// Configurable directories and extensions
#if WINVER > _WIN32_WINNT_NT4
	wstring wallDir = configUtil.getWallpaperDir();
	wstring malwareDir = configUtil.getMalwareDir();
	vector<wstring> openableExtensions = configUtil.getOpenableExtensions();
	vector<wstring> musicExtensions = configUtil.getMusicExtensions();
#else
	string wallDir = StringHelper::toLowercase(configUtil.getWallpaperDir());
	string malwareDir = StringHelper::toLowercase(configUtil.getMalwareDir());
	string bootImagesDir = StringHelper::toLowercase(configUtil.getBootImagesDir());
	string shutdownImagesDir = StringHelper::toLowercase(configUtil.getShutdownImagesDir());
	vector<string> openableExtensions = configUtil.getOpenableExtensions();
	vector<string> musicExtensions = configUtil.getMusicExtensions();
#endif

	// Start utilities
#if WINVER > _WIN32_WINNT_NT4
	Utils::FileUtil fileUtil(wallDir, malwareDir, openableExtensions, musicExtensions);
	wstring logFileName = fileUtil.workingDirectory + L"\\" + StringHelper::stringToWideString(Utils::TimeUtil::getAndFormatCurrentTime("%Y%m%d-%H%M")) + L"-logfile.log";
	wstring msgBoxProcessName = fileUtil.workingDirectory + L"\\message_box_process.exe";
#else
	Utils::FileUtil fileUtil(wallDir, malwareDir, bootImagesDir, shutdownImagesDir, openableExtensions, musicExtensions);
	string logFileName = fileUtil.workingDirectory + "\\" + Utils::TimeUtil::getAndFormatCurrentTime("%Y%m%d-%H%M") + "-logfile.log";
	string msgBoxProcessName = fileUtil.workingDirectory + "\\message_box_process.exe";
#endif

	Utils::LoggingUtil loggingUtil(logFileName, configUtil.getLoggingLevel());
	shared_ptr<Utils::LoggingUtil> sharedLoggingUtil = make_shared<Utils::LoggingUtil>(loggingUtil);
	shared_ptr<Utils::FileUtil> sharedFileUtil = make_shared<Utils::FileUtil>(fileUtil);
	Errors::ErrorHandler errorHandler(sharedLoggingUtil, msgBoxProcessName);
	unique_ptr<Errors::ErrorHandler> errorHandlerPtr = make_unique<Errors::ErrorHandler>(errorHandler);

	// Initial debug logs
#if WINVER > _WIN32_WINNT_NT4
	loggingUtil.debug("Working directory is " + StringHelper::wideStringToString(fileUtil.workingDirectory) + "\n");
	loggingUtil.debug("Wallpapers directory is " + StringHelper::wideStringToString(wallDir));
	loggingUtil.debug("Malware directory is " + StringHelper::wideStringToString(malwareDir));
#else
	loggingUtil.debug("Working directory is " + fileUtil.workingDirectory + "\n");
	loggingUtil.debug("Wallpapers directory is " + wallDir);
	loggingUtil.debug("Malware directory is " + malwareDir);
#endif
	// Starting server
	Server::WebServer webServer = WebServer(host, port, sharedFileUtil, sharedLoggingUtil, move(errorHandlerPtr));
	Server::Routes::RouteHandler routeHandler(configUtil, sharedFileUtil, sharedLoggingUtil, msgBoxProcessName, &themeMutex);

	webServer.addRoutes(routeHandler.getRoutes());
	try {
		HINSTANCE hInstance = GetModuleHandle(NULL);
		pthread_t thread;

		Threads::ThreadData threadData{
			hInstance, &webServer
		};
		loggingUtil.debug("Starting tray thread");
		pthread_create(&thread, NULL, Threads::TrayThread::trayIconThread, (void*)&threadData);
		pthread_detach(thread);
		loggingUtil.debug("Starting server");
		webServer.start();
		pthread_exit(0);
		loggingUtil.debug("Successfully exited");
		exit(0);
	} catch (string& e) {
		errorHandler.handleError(e);
	} catch (string* e) {
		errorHandler.handleError(*e);
	} catch (InteractBoxException& e) {
		errorHandler.handleError(e);
	} catch (InteractBoxException* e) {
		errorHandler.handleError(e);
	} catch (exception& e) {
		errorHandler.handleError(e);
	} catch (exception* e) {
		errorHandler.handleError(*e);
	}

#if WINVER > _WIN32_WINNT_NT4
	RemoveVectoredExceptionHandler(handler);
#endif
	return 0;
}