#include "server/routes/route_handlers.hpp"

namespace Server::Routes {

	using namespace std;

#if WINVER > _WIN32_WINNT_NT4
	wstring processThemeCommand(shared_ptr<Utils::FileUtil> fileUtil, shared_ptr<Utils::LoggingUtil> loggingUtil, pthread_mutex_t* themeMutex) {
#else
	string processThemeCommand(shared_ptr<Utils::FileUtil> fileUtil, shared_ptr<Utils::LoggingUtil> loggingUtil, pthread_mutex_t * themeMutex) {
#endif
		if (fileUtil->themeFiles.size() == 0) throw InteractBoxException(ErrorCodes::ThemeFilesNotFound);
#if WINVER > _WIN32_WINNT_NT4
		wstring file = fileUtil->selectRandomFile(L"theme");
#else
		string file = fileUtil->selectRandomFile("theme");
#endif
		ThemeArgs* themeArgs = new ThemeArgs{
			fileUtil,
			loggingUtil,
			file,
			themeMutex
		};
		pthread_t themeThread;
		pthread_create(&themeThread, NULL, runThemeThread, (void*)themeArgs);
		pthread_detach(themeThread);
		return file;
	}

#if WINVER > _WIN32_WINNT_NT4
	wstring processSoundCommand(shared_ptr<Utils::FileUtil> fileUtil) {
		vector<wstring> keys = {
			L"SystemStart",
			L"SystemExit",
			L"SystemAsterisk",
			L"SystemQuestion",
			L"SystemHand",
			L"SystemExclamation",
		};
#else
	string processSoundCommand(shared_ptr<Utils::FileUtil> fileUtil) {
		vector<string> keys = {
			"SystemStart",
			"SystemExit",
			"SystemAsterisk",
			"SystemQuestion",
			"SystemHand",
			"SystemExclamation",
		};
#endif
		auto packDirs = fileUtil->getSoundPacks();
		if (packDirs.size() == 0) throw InteractBoxException(ErrorCodes::SoundPacksNotFound);
		auto packDir = IndexHelper::getRandomItem(packDirs);
		auto files = FileHelper::filterFiles(fileUtil->files, packDir);
#if WINVER > _WIN32_WINNT_NT4
		if (packDir.ends_with(L"default")) {
			setDefaultSounds(keys);
			wstring pack = StringHelper::removeSuffix(StringHelper::removePrefix(packDir, fileUtil->workingDirectory + L"\\sounds\\"), L"_pack");
			fileUtil->setSoundPack(pack);
			return pack;
		}
		wstring jsonFile;
		for (auto& file : files) {
			if (!file.ends_with(L".json")) {
				continue;
			}
			jsonFile = file;
		}
#else
		if (packDir.ends_with("default")) {
			setDefaultSounds(keys);
			string pack = StringHelper::removeSuffix(StringHelper::removePrefix(packDir, fileUtil->workingDirectory + "\\sounds\\"), "_pack");
			fileUtil->setSoundPack(pack);
			return pack;
		}
		string jsonFile;
		for (auto& file : files) {
			if (!file.ends_with(".json")) {
				continue;
			}
			jsonFile = file;
		}
#endif
		if (jsonFile.empty()) throw InteractBoxException(ErrorCodes::CannotFindResource);
#if WINVER > _WIN32_WINNT_NT4
		string jsonString = FileHelper::readFileAsString(StringHelper::wideStringToString(jsonFile));
#else
		string jsonString = FileHelper::readFileAsString(jsonFile);
#endif
		Json::Value jsonData = JsonHelper::parseJsonString(jsonString);

#if WINVER > _WIN32_WINNT_NT4
		setSoundsFromJson(jsonData, packDir, keys);
		wstring pack = StringHelper::removeSuffix(StringHelper::removePrefix(packDir, fileUtil->workingDirectory + L"\\sounds\\"), L"_pack");
#else
		setSoundsFromJson(jsonData, packDir, keys);
		string pack = StringHelper::removeSuffix(StringHelper::removePrefix(packDir, fileUtil->workingDirectory + "\\sounds\\"), "_pack");
#endif
		fileUtil->setSoundPack(pack);
		return pack;
	}

#if WINVER > _WIN32_WINNT_NT4
	wstring processMalwareCommand(shared_ptr<Utils::FileUtil> fileUtil, shared_ptr<Utils::LoggingUtil> loggingUtil) {
#else
	string processMalwareCommand(shared_ptr<Utils::FileUtil> fileUtil, shared_ptr<Utils::LoggingUtil> loggingUtil) {
#endif
		if (fileUtil->malwareFiles.size() == 0) throw InteractBoxException(ErrorCodes::MalwareFilesNotFound);
		auto malwareFile = fileUtil->getRandomMalware();
#if WINVER > _WIN32_WINNT_NT4
		loggingUtil->debug("Malware file is " + StringHelper::wideStringToString(malwareFile));
#else
		loggingUtil->debug("Malware file is " + malwareFile);
#endif
		loggingUtil->debug("Reading dates JSON file...");
#if WINVER > _WIN32_WINNT_NT4
		string dateSettingsFile = StringHelper::wideStringToString(fileUtil->windowsDir);
#else
		string dateSettingsFile = fileUtil->windowsDir;
#endif
		dateSettingsFile += "\\malware_with_date_specific_deployment.json";
		string dateSpecificJsonString = FileHelper::readFileAsString(dateSettingsFile);
		loggingUtil->debug("Parsing dates JSON file...");
		Json::Value dateSpecificJson = JsonHelper::parseJsonString(dateSpecificJsonString);
		loggingUtil->debug("Checking dates...");

		int size = dateSpecificJson.size();
		for (int i = 0; i < size; i++) {
			string name = dateSpecificJson[i]["file"].asString();
			string dateString = dateSpecificJson[i]["date"].asString();
			if (boost::algorithm::istarts_with(malwareFile, name)) continue;
			vector<string> splitDateString = StringHelper::splitString(dateString, "-");
			Utils::TimeUtil::setSystemDateTime(atoi(splitDateString[0].c_str()), atoi(splitDateString[1].c_str()), atoi(splitDateString[2].c_str()));
		}
		fileUtil->openFile(malwareFile);
		return malwareFile;
	}

	vector<Http::HttpRoute> RouteHandler::getRoutes() {
		return _routes;
	}

#if WINVER == _WIN32_WINNT_NT4
	string setBootScreen(shared_ptr<Utils::FileUtil> fileUtil, bool isShutdownScreen = false) {
		string path = fileUtil->selectRandomFile(isShutdownScreen ? "shutdown" : "boot");
		FileHelper::copyFile(path, isShutdownScreen ? "C:\\WINDOWS\\LOGOW.SYS" : "C:\\LOGO.SYS");
		return path;
	}
#endif

	void RouteHandler::_setupRoutes() {
		_routes = { Http::HttpRoute(
		"/",
		"GET",
		[](Http::HttpRequest* req, Http::HttpResponse* res) {
			res->setResponse(nullopt, "pong", 200);
		}
	), Http::HttpRoute(
		"/baseData",
		"GET",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			map<string, int> resBody;
			resBody["total files"] = _fileUtil->files.size();
			resBody["malware files"] = _fileUtil->malwareFiles.size();
			resBody["wallpaper files"] = _fileUtil->wallpaperFiles.size();
			resBody["music files"] = _fileUtil->musicFiles.size();
			resBody["winamp skins"] = _fileUtil->winampSkinFiles.size();
			Json::Value jsonBody = JsonHelper::createJsonBody(resBody);
			res->setResponse(nullopt, jsonBody, 200);
		}
	), Http::HttpRoute(
		"/abort",
		"GET",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			#if WINVER > _WIN32_WINNT_NT4
			Utils::MessageBoxUtil::createBox(L"Exiting", L"Abort request received, Interact Box will now close", L"warn", L"ok");
			#else
			Utils::MessageBoxUtil::createBox("Exiting", "Abort request received, Interact Box will now close", "warn", "ok");
			#endif
			res->setResponse(nullopt, "Aborted", 200);
		}
	), Http::HttpRoute(
		"/box",
		"POST",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			processBoxRequest(_msgBoxProcessName, req->body, _fileUtil);
			res->setResponse(nullopt, "Opened box", 200);
		},
		_configUtil.getUseMessageBox()
	), Http::HttpRoute(
		"/sound",
		"GET",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			auto pack = processSoundCommand(_fileUtil);
			#if WINVER > _WIN32_WINNT_NT4
			Json::Value jsonBody = JsonHelper::createJsonBody("pack", StringHelper::wideStringToString(pack));
			Utils::MessageBoxUtil::createBox(L"New sounds set", L"Set sounds to " + pack, L"i", L"ok");
			#else
			Json::Value jsonBody = JsonHelper::createJsonBody("pack", pack);
			Utils::MessageBoxUtil::createBox("New sounds set", "Set sounds to " + pack, "i", "ok");
			#endif
			res->setResponse(nullopt, jsonBody, 200);
		},
		_configUtil.getUseSysSounds()
	), Http::HttpRoute(
		"/terminal",
		"POST",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			string command = JsonHelper::getJsonStringValue(req->body, "command");
			#if WINVER > _WIN32_WINNT_NT4
			Utils::CmdUtil::executeDosCommand(command, StringHelper::wideStringToString(_configUtil.getMalwareDir()), _configUtil.getWarnAboutUrlsInTerminal());
			DWORD processId = ProcessHelper::getProcessId(L"CMD.EXE");
			#else
			Utils::CmdUtil::executeDosCommand(command, _configUtil.getMalwareDir(), _configUtil.getWarnAboutUrlsInTerminal());
			DWORD processId = ProcessHelper::getProcessId("COMMAND.COM");
			#endif
			HWND window = ProcessHelper::findMainWindow(processId);
			ProcessHelper::setToForeground(window);
			res->setResponse(nullopt, req->body, 200);
		},
		_configUtil.getUseTerminal()
	), Http::HttpRoute(
		"/tts",
		"POST",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			#if WINVER > _WIN32_WINNT_NT4
			string input = JsonHelper::getJsonStringValue(req->body, "input");
			Utils::ShellUtil::openShell(L"tts_process.exe", L"open", _fileUtil->workingDirectory, L"\"" + StringHelper::stringToWideString(input) + L"\"");
			res->setResponse(nullopt, req->body, 200);
			#else
			throw InteractBoxException(ErrorCodes::UnsupportedFeature);
			#endif
		},
		_configUtil.getUseTts()
	), Http::HttpRoute(
		"/wallpaper",
		"GET",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			processWallpaperCommand(_fileUtil);
			res->setResponse(nullopt, "Set wallpaper", 200);
		},
		_configUtil.getUseWallpapers()
	), Http::HttpRoute(
		"/deleteRandom",
		"GET",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			auto deletedFile = _fileUtil->deleteRandomFile();
			#if WINVER > _WIN32_WINNT_NT4
			Json::Value jsonBody = JsonHelper::createJsonBody("file", StringHelper::wideStringToString(deletedFile).c_str());
			#else
			Json::Value jsonBody = JsonHelper::createJsonBody("file", deletedFile.c_str());
			#endif
			res->setResponse(nullopt, jsonBody, 200);
		},
		_configUtil.getUseDeleteRandom()
	), Http::HttpRoute(
		"/deleteSystem",
		"GET",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			#if WINVER > _WIN32_WINNT_NT4
			wstring triviaGamePath = _fileUtil->workingDirectory + L"\\trivia_game.exe";
			#else
			string triviaGamePath = _fileUtil->workingDirectory + "\\trivia_game.exe";
			#endif
			_fileUtil->openFile(triviaGamePath);
			ProcessHelper::setToForeground(triviaGamePath);
			res->setResponse(nullopt, "Opened trivia game", 200);
		},
		_configUtil.getUseDeleteSystem()
	), Http::HttpRoute(
		"/openRandom",
		"GET",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			auto file = _fileUtil->openRandomFile();
			ProcessHelper::setToForeground(file);
			#if WINVER > _WIN32_WINNT_NT4
			Json::Value jsonBody = JsonHelper::createJsonBody("file", StringHelper::wideStringToString(file).c_str());
			#else
			Json::Value jsonBody = JsonHelper::createJsonBody("file", file.c_str());
			#endif
			res->setResponse(nullopt, jsonBody, 200);
		},
		_configUtil.getUseOpenRandom()
	), Http::HttpRoute(
		"/music",
		"GET",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			#if WINVER > _WIN32_WINNT_NT4
			wstring file = _fileUtil->openRandomFile(L"music");
			#else
			string file = _fileUtil->openRandomFile("music");
			#endif
			ProcessHelper::setToForeground(file);
			#if WINVER > _WIN32_WINNT_NT4
			Json::Value jsonBody = JsonHelper::createJsonBody("file", StringHelper::wideStringToString(file).c_str());
			#else
			Json::Value jsonBody = JsonHelper::createJsonBody("file", file.c_str());
			#endif
			res->setResponse(nullopt, jsonBody, 200);
		},
		_configUtil.getUseMusic()
	), Http::HttpRoute(
		"/runMalware",
		"GET",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			auto malwareFile = processMalwareCommand(_fileUtil, _loggingUtil);
			ProcessHelper::setToForeground(malwareFile);
			#if WINVER > _WIN32_WINNT_NT4
			Json::Value jsonBody = JsonHelper::createJsonBody("file", StringHelper::wideStringToString(malwareFile).c_str());
			#else
			Json::Value jsonBody = JsonHelper::createJsonBody("file", malwareFile.c_str());
			#endif
			res->setResponse(nullopt, jsonBody, 200);
		},
		_configUtil.getUseMalware()
	), Http::HttpRoute(
		"/reboot",
		"GET",
		[](Http::HttpRequest* req, Http::HttpResponse* res) {
			Utils::RebootUtil::reboot();
			res->setResponse(nullopt, "Rebooted", 200);
		},
		_configUtil.getUseReboot()
	), Http::HttpRoute(
		"/crash",
		"GET",
		[](Http::HttpRequest* req, Http::HttpResponse* res) {
			Utils::CrashUtil::crash();
			res->setResponse(nullopt, "Crashed", 200);
		},
		_configUtil.getUseCrash()
	), Http::HttpRoute(
		"/macro",
		"GET",
		[](Http::HttpRequest* req, Http::HttpResponse* res) {
			Utils::ResolutionUtil::changeResolution(true);
			res->setResponse(nullopt, "Changed resolution", 200);
		},
		_configUtil.getUseMacroResolution()
	), Http::HttpRoute(
		"/micro",
		"GET",
		[](Http::HttpRequest* req, Http::HttpResponse* res) {
			Utils::ResolutionUtil::changeResolution(false);
			res->setResponse(nullopt, "Changed resolution", 200);
		},
		_configUtil.getUseMicroResolution()
	), Http::HttpRoute(
		"/colors",
		"POST",
		[](Http::HttpRequest* req, Http::HttpResponse* res) {
			int colors = JsonHelper::getJsonIntValue(req->body, "colors");
			Utils::ResolutionUtil::changeColors(colors);
			res->setResponse(nullopt, "ok", 200);
		},
		_configUtil.getUseColors()
	), Http::HttpRoute(
		"/theme",
		"GET",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			auto theme = processThemeCommand(_fileUtil, _loggingUtil, _themeMutex);
			#if WINVER > _WIN32_WINNT_NT4
			res->setResponse(nullopt, JsonHelper::createJsonBody("theme", StringHelper::wideStringToString(theme)), nullopt);
			#else
			res->setResponse(nullopt, JsonHelper::createJsonBody("theme", theme), nullopt);
			#endif
		},
		_configUtil.getUseThemes()
	), Http::HttpRoute(
		"/winamp",
		"GET",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			#if WINVER > _WIN32_WINNT_NT4
			wstring winampExecutable = L"C:\\PROGRAM FILES\\WINAMP\\WINAMP.EXE";
			#else
			string winampExecutable = "C:\\PROGRAM FILES\\WINAMP\\WINAMP.EXE";
			#endif
			auto file = IndexHelper::getRandomItem(_fileUtil->winampSkinFiles);
			_fileUtil->openFile(winampExecutable, file);
			#if WINVER > _WIN32_WINNT_NT4
			Json::Value jsonBody = JsonHelper::createJsonBody("file", StringHelper::wideStringToString(file).c_str());
			#else
			Json::Value jsonBody = JsonHelper::createJsonBody("file", file.c_str());
			#endif
			res->setResponse(nullopt, jsonBody, 200);
		},
		_configUtil.getUseWinamp()
	), Http::HttpRoute(
		"/bootImage",
		"POST",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			#if WINVER > _WIN32_WINNT_NT4
			throw InteractBoxException(ErrorCodes::UnsupportedFeature);
			#else
			bool isShutdownScreen = JsonHelper::getJsonBoolValue(req->body, "isShutdownScreen");
			_loggingUtil->debug("Directory is: " + isShutdownScreen ? _configUtil.getShutdownImagesDir() : _configUtil.getBootImagesDir());
			string file = setBootScreen(_fileUtil, isShutdownScreen);
			Json::Value jsonBody = JsonHelper::createJsonBody("file", file.c_str());
			res->setResponse(nullopt, jsonBody, 200);
			#endif
		},
		#if WINVER > _WIN32_WINNT_NT4
		nullopt
		#else
		_configUtil.getUseBootAndShutdownImages()
		#endif
	), Http::HttpRoute(
		"/systemBox",
		"POST",
		[this](Http::HttpRequest* req, Http::HttpResponse* res) {
			#if WINVER > _WIN32_WINNT_NT4
			throw InteractBoxException(ErrorCodes::UnsupportedFeature);
		},
		nullopt
			#else
			if (!FileHelper::checkIfFileExists(_fileUtil->workingDirectory + "\\system_box.exe")) {
				res->setResponse(nullopt, "{\"error\": \"Missing system box executable\"}", HttpStatus::FailedDependency);
				return;
			}
			string text = JsonHelper::getJsonStringValue(req->body, "text");
			if (text.size() > 126) {
				Utils::MessageBoxUtil::createBox(
					"Interact Box 98 WARNING",
					"The System Box redeem can only handle up to 126 characters. Input will be truncated",
					"w",
					"OK"
				);
				text = text.substr(0, 125);
			}
			Utils::ShellUtil::openShell("system_box.exe", "open", _fileUtil->workingDirectory, text);
			res->setResponse(nullopt, "ok", 200);
		},
		_configUtil.getUseSystemBox()
#endif
	), Http::HttpRoute(
		"/killInteractBox",
		"GET",
		[](Http::HttpRequest* req, Http::HttpResponse* res) {
			int* pBadPtr = nullptr;
			*pBadPtr = 42;
			res->setResponse(nullopt, "ok", 200);
		}
	) };
	}
}
