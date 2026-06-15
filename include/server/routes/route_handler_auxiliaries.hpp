#pragma once

#include "server/web_server.hpp"
#ifdef WIN32
	#include "utils/registry_util.hpp"
#else
	#include "utils/sudo_user_util.hpp"
#endif

namespace Server::Routes {
	struct FileAndDate {
			std::string file;
			std::string date;
	};

	struct ThemeArgs {
			std::shared_ptr<Utils::FileUtil> fileUtil;
			std::shared_ptr<Utils::LoggingUtil> loggingUtil;
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
			std::wstring randomFile;
#else
			std::string randomFile;
#endif
			pthread_mutex_t* themeMutex;
	};

	struct BaseDataItem {
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
			std::vector<std::wstring> files;
#else
			std::vector<std::string> files;
#endif
			int total;
	};

#ifdef WIN32
	BOOL CALLBACK getOkButton(HWND hwnd, LPARAM lParam);
	void* runThemeThread(void* arg);
	#if WINVER > _WIN32_WINNT_NT4
	void setSoundsFromJson(
		Json::Value jsonData,
		std::wstring packDir,
		std::vector<std::wstring> keys
	);
	void setDefaultSounds(std::vector<std::wstring> keys);
	void processBoxRequest(
		std::wstring processName,
		Json::Value jsonRequest,
		std::shared_ptr<Utils::FileUtil> fileUtil
	);
	#else
	void setSoundsFromJson(Json::Value jsonData, std::string packDir, std::vector<std::string> keys);
	void setDefaultSounds(std::vector<std::string> keys);
	void processBoxRequest(
		std::string processName,
		Json::Value jsonRequest,
		std::shared_ptr<Utils::FileUtil> fileUtil
	);
	#endif
	void processWallpaperCommand(std::shared_ptr<Utils::FileUtil> fileUtil);
#else
	const std::string getDesktop();
	void setSoundsFromJson(Json::Value jsonData, std::string packDir, std::vector<std::string> keys);
	void setDefaultSounds(std::vector<std::string> keys);
	void processWallpaperCommand(std::shared_ptr<Utils::FileUtil> fileUtil, std::shared_ptr<Utils::SudoUserUtil> sudoUserUtil);
	void processBoxRequest(
		std::string processName,
		Json::Value jsonRequest,
		std::shared_ptr<Utils::FileUtil> fileUtil
	);
#endif
} // namespace Server::Routes