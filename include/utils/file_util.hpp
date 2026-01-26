#pragma once
#include "utils.hpp"
#include "general_definitions.hpp"

#define MAX_FILE_VEC_SIZE 1000000

namespace Utils {
	struct DirAndVector {
#if WINVER > _WIN32_WINNT_NT4
			std::wstring dir;
			std::vector<std::wstring>* vec;
#else
			std::string dir;
			std::vector<std::string>* vec;
#endif
	};
	struct ExtensionsAndVector {
#if WINVER > _WIN32_WINNT_NT4
			std::vector<std::wstring>* extensions;
			std::vector<std::wstring>* vec;
#else
			std::vector<std::string>* extensions;
			std::vector<std::string>* vec;
#endif
	};
	class FileUtil {

		public:
#if WINVER > _WIN32_WINNT_NT4
			FileUtil(
				std::wstring wallDir,
				std::wstring malwareDir,
				std::vector<std::wstring> openableExtensions,
				std::vector<std::wstring> musicExtensions
			);

			std::vector<std::wstring> files;
			std::wstring workingDirectory;
			std::vector<std::wstring> malwareFiles;
			std::vector<std::wstring> musicFiles;
			std::vector<std::wstring> wallpaperFiles;
			std::vector<std::wstring> winampSkinFiles;
			std::vector<std::wstring> themeFiles;
			std::vector<std::wstring> openableFiles;
			std::wstring windowsDir;

			std::wstring setWallpaper(std::wstring currentWallpaper);
			std::wstring getRandomMalware();
			std::wstring deleteRandomFile();
			std::vector<std::wstring> getSoundPacks();
			std::wstring openRandomFile(std::optional<std::wstring> type = std::nullopt, int retries = 0);
			std::wstring selectRandomFile(
				std::optional<std::wstring> type = std::nullopt,
				int retries = 0
			);
			void openFile(std::wstring file);
			void openFile(std::wstring file, std::wstring parameters);
			void setSoundPack(std::wstring pack);
#else
			FileUtil(
				std::string wallDir,
				std::string malwareDir,
				std::string bootImagesDir,
				std::string shutdownImagesDir,
				std::vector<std::string> openableExtensions,
				std::vector<std::string> musicExtensions
			);

			std::vector<std::string> files;
			std::string workingDirectory;
			std::vector<std::string> malwareFiles;
			std::vector<std::string> musicFiles;
			std::vector<std::string> wallpaperFiles;
			std::vector<std::string> bootImageFiles;
			std::vector<std::string> shutdownImagesFiles;
			std::vector<std::string> winampSkinFiles;
			std::vector<std::string> themeFiles;
			std::vector<std::string> openableFiles;
			std::string windowsDir;

			std::string setWallpaper(std::string currentWallpaper);
			std::string getRandomMalware();
			std::string deleteRandomFile();
			std::vector<std::string> getSoundPacks();
			std::string openRandomFile(std::optional<std::string> type = std::nullopt, int retries = 0);
			std::string selectRandomFile(std::optional<std::string> type = std::nullopt, int retries = 0);
			void openFile(std::string file);
			void openFile(std::string file, std::string parameters);
			void setSoundPack(std::string pack);
#endif

		private:
#if WINVER > _WIN32_WINNT_NT4
			std::wstring activeSoundPack;
			std::map<std::wstring, std::vector<std::wstring>*> _mapOfOpenableOptions;
#else
			std::string activeSoundPack;
			std::map<std::string, std::vector<std::string>*> _mapOfOpenableOptions;
#endif

			void _listFiles(
#if WINVER > _WIN32_WINNT_NT4
				const std::wstring& directory,
#else
				const std::string& directory,
#endif
				std::vector<DirAndVector> dirsAndVectors,
				std::vector<ExtensionsAndVector> extensionsAndVectors
			);
			void _addToOpenableOptions(
#if WINVER > _WIN32_WINNT_NT4
				std::wstring& fullPath,
#else
				std::string& fullPath,
#endif
				std::vector<DirAndVector> dirsAndVectors,
				std::vector<ExtensionsAndVector> extensionsAndVectors
			);

#if WINVER > _WIN32_WINNT_NT4
			std::vector<std::wstring> checkDirAndFilterFiles(std::wstring dir, std::wregex pattern);
			std::wstring _getFullPath(std::wstring dir, std::wregex pattern);
			std::tuple<std::wstring, int> _getRandomFile(
				std::vector<std::wstring> files,
				bool isDefaultFiles = false
			);

#else
			std::vector<std::string> checkDirAndFilterFiles(std::string dir, std::regex pattern);
			std::string _getFullPath(std::string dir, std::regex pattern);
			std::tuple<std::string, int> _getRandomFile(
				std::vector<std::string> files,
				bool isDefaultFiles = false
			);
#endif
	};
} // namespace Utils
