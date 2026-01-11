#include "utils/config_util.hpp"

namespace Utils {
	using namespace std;

	ConfigUtil::ConfigUtil(string configFileName) {
		try {
			string contents = FileHelper::readFileAsString(configFileName);
			Json::Value jsonContents = JsonHelper::parseJsonString(contents);
			#if WINVER > _WIN32_WINNT_NT4
			_host = JsonHelper::getJsonWideStringValue(jsonContents, "host");
			
			#else
			_host = JsonHelper::getJsonStringValue(jsonContents, "host");
			#endif
			_port = JsonHelper::getJsonIntValue(jsonContents, "port");

			_useWallpapers = JsonHelper::getJsonBoolValue(jsonContents, "useWallpapers");
			_useMusic = JsonHelper::getJsonBoolValue(jsonContents, "useMusic");
			_useMalware = JsonHelper::getJsonBoolValue(jsonContents, "useMalware");
			_useWinamp = JsonHelper::getJsonBoolValue(jsonContents, "useWinamp");
			_useSysSounds = JsonHelper::getJsonBoolValue(jsonContents, "useSysSounds");
			_useTerminal = JsonHelper::getJsonBoolValue(jsonContents, "useTerminal");
			_useMessageBox = JsonHelper::getJsonBoolValue(jsonContents, "useMessageBox");
			_useOpenRandom = JsonHelper::getJsonBoolValue(jsonContents, "useOpenRandom");
			_useDeleteRandom = JsonHelper::getJsonBoolValue(jsonContents, "useDeleteRandom");
			_useDeleteSystem = JsonHelper::getJsonBoolValue(jsonContents, "useDeleteSystem");
			_useReboot = JsonHelper::getJsonBoolValue(jsonContents, "useReboot");
			_useCrash = JsonHelper::getJsonBoolValue(jsonContents, "useCrash");
			_useMacroResolution = JsonHelper::getJsonBoolValue(jsonContents, "useMacroResolution");
			_useMicroResolution = JsonHelper::getJsonBoolValue(jsonContents, "useMicroResolution");
			_useColors = JsonHelper::getJsonBoolValue(jsonContents, "useColors");
			_useThemes = JsonHelper::getJsonBoolValue(jsonContents, "useThemes");
			_warnAboutUrlsInTerminal = JsonHelper::getJsonBoolValue(jsonContents, "warnAboutUrlsInTerminal");
			
			
			#if WINVER > _WIN32_WINNT_NT4
			_useTts = JsonHelper::getJsonBoolValue(jsonContents, "useTts");
			_wallpaperDir = JsonHelper::getJsonWideStringValue(jsonContents, "wallpaperDir");
			_malwareDir = JsonHelper::getJsonWideStringValue(jsonContents, "malwareDir");
			_musicExtensions = _convertVector(JsonHelper::getJsonStringArray(jsonContents, "musicExtensions"));
			_openableExtensions = _convertVector(JsonHelper::getJsonStringArray(jsonContents, "openableExtensions"));
	
			#else
			_wallpaperDir = JsonHelper::getJsonStringValue(jsonContents, "wallpaperDir");
			_malwareDir = JsonHelper::getJsonStringValue(jsonContents, "malwareDir");
			_bootImagesDir = JsonHelper::getJsonStringValue(jsonContents, "bootImagesDir");
			_shutdownImagesDir = JsonHelper::getJsonStringValue(jsonContents, "shutdownImagesDir");
	
			_musicExtensions = JsonHelper::getJsonStringArray(jsonContents, "musicExtensions");
			_openableExtensions = JsonHelper::getJsonStringArray(jsonContents, "openableExtensions");
			_useBootAndShutdownImages = JsonHelper::getJsonBoolValue(jsonContents, "useBootAndShutdownImages");
			_useSystemBox = JsonHelper::getJsonBoolValue(jsonContents, "useSystemBox");
			#endif
			_loggingLevel = static_cast<LoggingLevel>(JsonHelper::getJsonIntValue(jsonContents, "loggingLevel"));
		} catch (InteractBoxException& e) {
			MessageBoxA(
				NULL,
				("Interact Box encountered the following error while reading your settings, and is unable to continue: " + e.what()).c_str(),
				"Interact Box Fatal Error",
				MB_ICONERROR
			);
			exit(1);
		}
	}

	
	int ConfigUtil::getPort() {
		return _port;
	}

	bool ConfigUtil::getUseWallpapers() {
		return _useWallpapers;
	}

	bool ConfigUtil::getUseMusic() {
		return _useMusic;
	}

	bool ConfigUtil::getUseMalware() {
		return _useMalware;
	}

	bool ConfigUtil::getUseWinamp() {
		return _useWinamp;
	}

	bool ConfigUtil::getUseSysSounds() {
		return _useSysSounds;
	}

	bool ConfigUtil::getUseTerminal() {
		return _useTerminal;
	}

	bool ConfigUtil::getUseMessageBox() {
		return _useMessageBox;
	}

	bool ConfigUtil::getUseOpenRandom() {
		return _useOpenRandom;
	}

	bool ConfigUtil::getUseDeleteRandom() {
		return _useDeleteRandom;
	}

	bool ConfigUtil::getUseDeleteSystem() {
		return _useDeleteSystem;
	}

	bool ConfigUtil::getUseReboot() {
		return _useReboot;
	}

	bool ConfigUtil::getUseCrash() {
		return _useCrash;
	}

	bool ConfigUtil::getUseMacroResolution() {
		return _useMacroResolution;
	}

	bool ConfigUtil::getUseMicroResolution() {
		return _useMicroResolution;
	}

	bool ConfigUtil::getUseColors() {
		return _useColors;
	}

	bool ConfigUtil::getUseThemes() {
		return _useThemes;
	}

	bool ConfigUtil::getUseTts() {
    return _useTts;
  }

  bool ConfigUtil::getWarnAboutUrlsInTerminal() {
    return _warnAboutUrlsInTerminal;
  }

	#if WINVER > _WIN32_WINNT_NT4
	wstring ConfigUtil::getWallpaperDir() {
		return _wallpaperDir;
	}
	
	wstring ConfigUtil::getMalwareDir() {
		return _malwareDir;
	}

	vector<wstring> ConfigUtil::getMusicExtensions() {
		return _musicExtensions;
	}

	vector<wstring> ConfigUtil::getOpenableExtensions() {
		return _openableExtensions;
	}

	vector<wstring> ConfigUtil::_convertVector(vector<string> input) {
		vector<wstring> result;
		for (auto& item : input) {
			result.push_back(StringHelper::stringToWideString(item));
		}
		return result;
	}

	wstring ConfigUtil::getHost() {
		return _host;
	}

	#else
	string ConfigUtil::getHost() {
		return _host;
	}

	string ConfigUtil::getWallpaperDir() {
		return _wallpaperDir;
	}

	string ConfigUtil::getMalwareDir() {
		return _malwareDir;
	}

  string ConfigUtil::getBootImagesDir() {
    return _bootImagesDir;
  }

	string ConfigUtil::getShutdownImagesDir() {
		return _shutdownImagesDir;
	}

	vector<string> ConfigUtil::getMusicExtensions() {
		return _musicExtensions;
	}

	vector<string> ConfigUtil::getOpenableExtensions() {
		return _openableExtensions;
	}

	bool ConfigUtil::getUseSystemBox() {
    return _useSystemBox;
  }

	bool ConfigUtil::getUseBootAndShutdownImages() {
    return _useBootAndShutdownImages;
  }

	#endif

	LoggingLevel ConfigUtil::getLoggingLevel() {
		return _loggingLevel;
	}
}