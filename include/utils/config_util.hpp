#pragma once
#include "utils.hpp"
#include "file_util.hpp"
#include "logging_level.hpp"

namespace Utils {
	class ConfigUtil {
		public:
			ConfigUtil(std::string configFileName);
			int getPort();
			bool getUseWallpapers();
			bool getUseMusic();
			bool getUseMalware();
			bool getUseWinamp();
			bool getUseSysSounds();
			bool getUseTerminal();
			bool getUseMessageBox();
			bool getUseOpenRandom();
			bool getUseDeleteRandom();
			bool getUseDeleteSystem();
			bool getUseReboot();
			bool getUseCrash();
			bool getUseMacroResolution();
			bool getUseMicroResolution();
			bool getUseColors();
			bool getUseThemes();
			bool getUseTts();
			bool getWarnAboutUrlsInTerminal();
			bool getUseBootAndShutdownImages();
			bool getUseSystemBox();
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
			std::wstring getHost();
			std::wstring getWallpaperDir();
			std::wstring getMalwareDir();
			std::vector<std::wstring> getMusicExtensions();
			std::vector<std::wstring> getOpenableExtensions();
#else
			std::string getHost();
			std::string getWallpaperDir();
			std::string getMalwareDir();
			std::string getBootImagesDir();
			std::string getShutdownImagesDir();
			std::vector<std::string> getMusicExtensions();
			std::vector<std::string> getOpenableExtensions();
#endif
			LoggingLevel getLoggingLevel();

		private:
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
			std::wstring _host;
#else
			std::string _host;

#endif
			int _port;
			bool _useBootAndShutdownImages;
			bool _useSystemBox;
			bool _useWallpapers;
			bool _useMusic;
			bool _useMalware;
			bool _useWinamp;
			bool _useSysSounds;
			bool _useTerminal;
			bool _useMessageBox;
			bool _useOpenRandom;
			bool _useDeleteRandom;
			bool _useDeleteSystem;
			bool _useReboot;
			bool _useCrash;
			bool _useMacroResolution;
			bool _useMicroResolution;
			bool _useThemes;
			bool _useTts;
			bool _useColors;
			bool _warnAboutUrlsInTerminal;
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
			std::wstring _wallpaperDir;
			std::wstring _malwareDir;
			std::vector<std::wstring> _musicExtensions;
			std::vector<std::wstring> _openableExtensions;
			std::vector<std::wstring> _convertVector(std::vector<std::string> input);
#else
			std::string _wallpaperDir;
			std::string _malwareDir;
			std::string _bootImagesDir;
			std::string _shutdownImagesDir;
			std::vector<std::string> _musicExtensions;
			std::vector<std::string> _openableExtensions;
#endif
			LoggingLevel _loggingLevel;
	};
} // namespace Utils