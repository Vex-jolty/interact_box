#pragma once
#ifdef WIN32
	#include <sdkddkver.h>

	#if WINVER < _WIN32_WINNT_VISTA
		#define _USE_32BIT_TIME_T 1
	#endif

	#include <winsock2.h>
	#include <windows.h>
#endif
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <pthread.h>
#ifdef WIN32
	#include <dbghelp.h>
#endif
#include <boost/algorithm/string.hpp>

#if defined(WIN32) && WINVER >= _WIN32_WINNT_VISTA
	#include <wbemcli.h>
	#include <comdef.h>
#endif

#include "utils/config_util.hpp"
#include "utils/crash_util.hpp"
#include "utils/message_box_util.hpp"
#include "utils/file_util.hpp"
#include "utils/registry_util.hpp"
#include "utils/time_util.hpp"
#include "utils/reboot_util.hpp"
#include "utils/logging_util.hpp"
#include "utils/cmd_util.hpp"
#include "utils/resolution_util.hpp"

#include "server/web_server.hpp"
#include "http/http_route.hpp"
#include "http/http_response.hpp"
#include "server/routes/route_handlers.hpp"

#include "errors/error_handler.hpp"

#include "./resources.h"

#ifdef WIN32
	#include "threads/tray_thread.hpp"
#endif

#include "exported.hpp"

struct FileAndDate {
		std::string file;
		std::string date;
};

struct ThemeArgs {
		std::shared_ptr<Utils::FileUtil> fileUtil;
		std::shared_ptr<Utils::LoggingUtil> loggingUtil;
		std::wstring randomFile;
};

struct BaseDataItem {
		std::vector<std::wstring> files;
		int total;
};

#if WINVER >= _WIN32_WINNT_VISTA
struct AntivirusInfo {
		std::wstring name;
		std::wstring exePath;
		uint32_t productState;
};
#endif