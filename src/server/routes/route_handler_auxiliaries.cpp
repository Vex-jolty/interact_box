#include "server/routes/route_handler_auxiliaries.hpp"

namespace Server::Routes {
	using namespace std;

#ifdef WIN32

	#if WINVER > _WIN32_WINNT_NT4
	void setSoundsFromJson(Json::Value jsonData, wstring packDir, vector<wstring> keys) {
		for (auto& key : keys) {
			Json::Value jsonValue = jsonData[StringHelper::wideStringToString(key)];
			wstring value = StringHelper::stringToWideString(jsonValue.asString());
			wstring subKey = L"AppEvents\\Schemes\\Apps\\.Default\\" + key + L"\\.Current";
			wstring file = packDir + L"\\" + value;
			bool success = Utils::RegistryUtil::setNewKeyValue(HKEY_CURRENT_USER, subKey, nullopt, file);
			if (!success)
				throw InteractBoxException(ErrorCodes::ErrorCode::CannotSetAudioFile, key + L" as " + value);
		}
	}

	void setDefaultSounds(vector<wstring> keys) {
		for (auto& key : keys) {
			wstring defaultValKey = L"AppEvents\\Schemes\\Apps\\.Default\\" + key;
			wstring value =
				Utils::RegistryUtil::getKeyValue(HKEY_CURRENT_USER, defaultValKey, L"\\.Default");
			wstring subKey = L"AppEvents\\Schemes\\Apps\\.Default\\" + key + L"\\.Current";
			bool success = Utils::RegistryUtil::setNewKeyValue(HKEY_CURRENT_USER, subKey, nullopt, value);
			if (!success)
				throw InteractBoxException(ErrorCodes::ErrorCode::CannotSetAudioFile, key + L" as " + value);
		}
	}

	#else
	void setSoundsFromJson(Json::Value jsonData, string packDir, vector<string> keys) {
		for (auto& key : keys) {
			Json::Value jsonValue = jsonData[key];
			string value = jsonValue.asString();
			string subKey = "AppEvents\\Schemes\\Apps\\.Default\\" + key + "\\.Current";
			string file = packDir + "\\" + value;
			bool success = Utils::RegistryUtil::setNewKeyValue(HKEY_CURRENT_USER, subKey, nullopt, file);
			if (!success)
				throw InteractBoxException(ErrorCodes::ErrorCode::CannotSetAudioFile, key + " as " + value);
		}
	}

	void setDefaultSounds(vector<string> keys) {
		for (auto& key : keys) {
			string defaultValKey = "AppEvents\\Schemes\\Apps\\.Default\\" + key;
			string value =
				Utils::RegistryUtil::getKeyValue(HKEY_CURRENT_USER, defaultValKey, "\\.Default");
			string subKey = "AppEvents\\Schemes\\Apps\\.Default\\" + key + "\\.Current";
			bool success = Utils::RegistryUtil::setNewKeyValue(HKEY_CURRENT_USER, subKey, nullopt, value);
			if (!success)
				throw InteractBoxException(ErrorCodes::ErrorCode::CannotSetAudioFile, key + " as " + value);
		}
	}
	#endif

	BOOL CALLBACK getOkButton(HWND hwnd, LPARAM lParam) {
		try {
			HWND* okButtonPtr = reinterpret_cast<HWND*>(lParam);
	#if WINVER > _WIN32_WINNT_NT4
			wchar_t name[256];
			wchar_t text[256];
			GetClassName(hwnd, name, sizeof(name));
			GetWindowText(hwnd, text, sizeof(text));
			if (boost::icontains(name, L"Button")) {
				if (boost::icontains(text, L"OK")) {
					*okButtonPtr = hwnd;
					return FALSE;
				}
			}
			return TRUE;
		} catch (exception& e) {
			MessageBox(NULL, L"Failed during enumeration", L"", MB_ICONERROR);
			return TRUE;
		}
	#else
			char name[100];
			char text[100];
			GetClassNameA(hwnd, name, sizeof(name));
			GetWindowTextA(hwnd, text, sizeof(text));
			if (((string)name).starts_with("Button")) {
				if (((string)text).starts_with("OK")) {
					*okButtonPtr = hwnd;
					return FALSE;
				}
			}
			return TRUE;
		} catch (exception& e) {
			MessageBoxA(NULL, "Failed during enumeration", "", MB_ICONERROR);
			return TRUE;
		}
	#endif
	}

	void* runThemeThread(void* arg) {
		// Lock to ensure this doesn't cause a race condition
		ThemeArgs* themeArgs = static_cast<ThemeArgs*>(arg);
		int lockResult = pthread_mutex_lock(themeArgs->themeMutex);
		shared_ptr<Utils::FileUtil> fileUtil = themeArgs->fileUtil;
		shared_ptr<Utils::LoggingUtil> loggingUtil = themeArgs->loggingUtil;
		try {
			if (lockResult != 0)
				throw InteractBoxException(ErrorCodes::ErrorCode::CannotLockMutex);
			auto file = themeArgs->randomFile;
			fileUtil->openFile(file);
			Sleep(1000);

			// Windows 7 seems to not open a display settings window, so we're skipping this logic for it

	#if WINVER < _WIN32_WINNT_7

		#if WINVER > _WIN32_WINNT_NT4
			HWND window = ProcessHelper::findDisplaySettingsWindow();
		#else
			HWND window = ProcessHelper::findMainWindow("C:\\PROGRAM FILES\\PLUS!\\THEMES.EXE");
		#endif
			for (int i = 0; i < 5 && window == NULL; ++i) {
				Sleep(500);
		#if WINVER > _WIN32_WINNT_NT4
				window = ProcessHelper::findDisplaySettingsWindow();
		#else
				window = ProcessHelper::findMainWindow("C:\\PROGRAM FILES\\PLUS!\\THEMES.EXE");
		#endif
			}
			WaitForInputIdle(window, INFINITE);
			if (window == NULL)
				throw InteractBoxException(ErrorCodes::ErrorCode::CannotFindWindow);
			ProcessHelper::setToForeground(window);
			// Create one HWND per call instead of using global variable
			HWND okButton = NULL;
			EnumChildWindows(window, getOkButton, reinterpret_cast<LPARAM>(&okButton));
			if (okButton == NULL)
				throw InteractBoxException(ErrorCodes::ErrorCode::CannotFindButton);
			SendMessage(okButton, BM_CLICK, 0, 0);
			// Waiting for window to close
			while (IsWindow(window)) {
				Sleep(500);
			}
	#endif
		} catch (InteractBoxException& e) {
			loggingUtil->err(e.what());
	#if WINVER > _WIN32_WINNT_NT4
			Utils::MessageBoxUtil::createBox(
				L"INTERACT BOX ERROR", StringHelper::stringToWideString(e.what()), L"e", L"ok"
			);
	#else
			Utils::MessageBoxUtil::createBox("INTERACT BOX ERROR", e.what(), "e", "ok");
	#endif
		} catch (exception& e) {
			string errMessage = e.what();
			loggingUtil->err(errMessage);
	#if WINVER > _WIN32_WINNT_NT4
			Utils::MessageBoxUtil::createBox(
				L"INTERACT BOX ERROR",
				L"Error while setting theme: " + StringHelper::stringToWideString(errMessage), L"e", L"ok"
			);
	#else
			Utils::MessageBoxUtil::createBox(
				"INTERACT BOX ERROR", "Error while setting theme: " + errMessage, "e", "ok"
			);
	#endif
		}
		// Unlock and finish
		// Outside try block to ensure unlocking even if error is thrown
		pthread_mutex_unlock(themeArgs->themeMutex);
		return nullptr;
	}

	#if WINVER > _WIN32_WINNT_NT4
	void processBoxRequest(
		wstring processName,
		Json::Value jsonRequest,
		shared_ptr<Utils::FileUtil> fileUtil
	) {
		wstring title = JsonHelper::getJsonWideStringValue(jsonRequest, L"title", "Interact Box");
		wstring content =
			JsonHelper::getJsonWideStringValue(jsonRequest, L"content", jsonRequest.toStyledString());
		wstring type = JsonHelper::getJsonWideStringValue(jsonRequest, L"type", "i");
		wstring buttons = JsonHelper::getJsonWideStringValue(jsonRequest, L"buttons", "ok");
		Utils::MessageBoxUtil::createBox(title, content, type, buttons);
	}

	void processWallpaperCommand(shared_ptr<Utils::FileUtil> fileUtil) {
		if (fileUtil->wallpaperFiles.size() == 0)
			throw InteractBoxException(ErrorCodes::ErrorCode::WallpapersNotFound);
		HKEY hKey = HKEY_CURRENT_USER;
		wstring topKey = L"Control Panel\\Desktop";
		wstring mainKey = L"Wallpaper";
		wstring currentWallpaper = Utils::RegistryUtil::getKeyValue(hKey, topKey, mainKey);
		wstring path = fileUtil->setWallpaper(currentWallpaper);
		Utils::RegistryUtil::setNewKeyValue(hKey, topKey, mainKey, path);
	}

	#else

	void processBoxRequest(
		string processName,
		Json::Value jsonRequest,
		shared_ptr<Utils::FileUtil> fileUtil
	) {
		string title = JsonHelper::getJsonStringValue(jsonRequest, "title", "Interact Box");
		string content =
			JsonHelper::getJsonStringValue(jsonRequest, "content", jsonRequest.toStyledString());
		string type = JsonHelper::getJsonStringValue(jsonRequest, "type", "i");
		string buttons = JsonHelper::getJsonStringValue(jsonRequest, "buttons", "ok");
		Utils::MessageBoxUtil::createBox(title, content, type, buttons);
	}

	void processWallpaperCommand(shared_ptr<Utils::FileUtil> fileUtil) {
		if (fileUtil->wallpaperFiles.size() == 0)
			throw InteractBoxException(ErrorCodes::ErrorCode::WallpapersNotFound);
		HKEY hKey = HKEY_CURRENT_USER;
		string topKey = "Control Panel\\Desktop";
		string mainKey = "Wallpaper";
		string currentWallpaper = Utils::RegistryUtil::getKeyValue(hKey, topKey, mainKey);
		string path = fileUtil->setWallpaper(currentWallpaper);
		Utils::RegistryUtil::setNewKeyValue(hKey, topKey, mainKey, path);
	}

	#endif

#else
	const string getDesktop() {
		string desktop = getenv("XDG_CURRENT_DESKTOP");
		boost::to_lower(desktop);
		boost::trim(desktop);
		return desktop.c_str();
	}

	const string getWallpaperCommand(bool isDark = false) {
	#ifdef INTERACT_BOX_GUI_QT
		return "qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScript \"var "
					 "Desktops = desktops();for (i=0;i<Desktops.length;i++) {d = "
					 "Desktops[i];d.wallpaperPlugin = \\\"org.kde.image\\\";d.currentConfigGroup "
					 "=Array(\\\"Wallpaper\\\",\\\"org.kde.image\\\",\\\"General\\\");print(d.readConfig("
					 "\\\"Image\\\"));}\"";
	#else
		const string desktop = getDesktop();
		if (desktop.starts_with("xfce")) {
			return "xfconf-query -c xfce4-desktop -p /backdrop/screen0/monitor0/workspace0/last-image";
		} else {
			const string endingPart = isDark ? "picture-uri-dark" : "picture-uri";
			return "gsettings get org.gnome.desktop.background " + endingPart;
		}
	#endif
	}

	const string getCurrentWallpaper(shared_ptr<Utils::SudoUserUtil> sudoUserUtil, bool isDark = false) {
		string command = getWallpaperCommand(isDark);
		const string& realUser = sudoUserUtil->getRealUser();
		if (!realUser.empty()) {
			command = "sudo -u " + realUser + " " + command;
		}
		const string outputPath = "/tmp/wallpaper_output.txt";
		command += " > " + outputPath;
		
		ifstream in(outputPath);
		string result;
		getline(in, result);

		boost::trim(result);

		if (result.empty() && command.starts_with("gsettings") && !isDark) {
			return getCurrentWallpaper(sudoUserUtil, isDark);
		}
		return result;
	}

	void processWallpaperCommand(shared_ptr<Utils::FileUtil> fileUtil, shared_ptr<Utils::SudoUserUtil> sudoUserUtil) {
		if (fileUtil->wallpaperFiles.size() == 0)
			throw InteractBoxException(ErrorCodes::ErrorCode::WallpapersNotFound);
		const string currentWallpaper = getCurrentWallpaper(sudoUserUtil);
		const string& wallpaper = fileUtil->setWallpaper(currentWallpaper);
	}

	void processBoxRequest(
		string processName,
		Json::Value jsonRequest,
		shared_ptr<Utils::FileUtil> fileUtil
	) {
		string title = JsonHelper::getJsonStringValue(jsonRequest, "title", "Interact Box");
		string content =
			JsonHelper::getJsonStringValue(jsonRequest, "content", jsonRequest.toStyledString());
		string type = JsonHelper::getJsonStringValue(jsonRequest, "type", "i");
		string buttons = JsonHelper::getJsonStringValue(jsonRequest, "buttons", "ok");
		Utils::MessageBoxUtil::createBox(title, content, type, buttons);
	}
#endif
} // namespace Server::Routes