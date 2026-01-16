#include "utils/file_util.hpp"

namespace Utils {
	using namespace std;
#if WINVER > _WIN32_WINNT_NT4
	FileUtil::FileUtil(
		wstring wallDir,
		wstring malwareDir,
		vector<wstring> openableExtensions,
		vector<wstring> musicExtensions
	) {
		workingDirectory = StringHelper::toLowercase(FileHelper::getWorkingDirectory());

		// Getting Windows directory
		windowsDir = FileHelper::getWindowsDirectory();

		wregex diskPattern(LR"([A-Za-z]:)");
		wallDir = _getFullPath(wallDir, diskPattern);
		malwareDir = _getFullPath(malwareDir, diskPattern);
		vector<wstring> themeExtension = {L".theme"};
		vector<wstring> winampExtension = {L".wsz"};
		DirAndVector wallDirAndVec = {wallDir, &wallpaperFiles};
		DirAndVector malwareDirAndVec = {malwareDir, &malwareFiles};
		ExtensionsAndVector openableExtensionsAndVec = {&openableExtensions, &openableFiles};
		ExtensionsAndVector musicExtensionsAndVec = {&musicExtensions, &musicFiles};
		ExtensionsAndVector themeExtensionAndVec = {&themeExtension, &themeFiles};
		ExtensionsAndVector winampExtensionAndVec = {&winampExtension, &winampSkinFiles};
		vector<DirAndVector> dirsAndVectors = {wallDirAndVec, malwareDirAndVec};
		vector<ExtensionsAndVector> extensionsAndVectors = {
			openableExtensionsAndVec, winampExtensionAndVec, musicExtensionsAndVec, themeExtensionAndVec
		};
		_listFiles(L"C:", dirsAndVectors, extensionsAndVectors);

		activeSoundPack = L"default";

		vector<wstring> bmpExtension = {L".bmp"};
		wallpaperFiles = FileHelper::filterFiles(wallpaperFiles, bmpExtension);
		_mapOfOpenableOptions[L"default"] = &openableFiles;
		_mapOfOpenableOptions[L"malware"] = &malwareFiles;
		_mapOfOpenableOptions[L"music"] = &musicFiles;
		_mapOfOpenableOptions[L"wallpaper"] = &wallpaperFiles;
		_mapOfOpenableOptions[L"winamp"] = &winampSkinFiles;
		_mapOfOpenableOptions[L"theme"] = &themeFiles;
	}

	// Private

	void FileUtil::_listFiles(
		const wstring &directory,
		vector<DirAndVector> dirsAndVectors,
		vector<ExtensionsAndVector> extensionsAndVectors
	) {
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile((directory + L"\\*").c_str(), &findFileData);

		if (hFind == INVALID_HANDLE_VALUE) {
			return;
		}

		do {
			const wstring fileOrDirectory = findFileData.cFileName;
			if (fileOrDirectory == L"." || fileOrDirectory == L"..") {
				continue;
			}
			wstring fullPath = StringHelper::toLowercase(directory + L"\\" + fileOrDirectory);

			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				_listFiles(fullPath, dirsAndVectors, extensionsAndVectors);
			} else {
				files.push_back(fullPath);
				_addToOpenableOptions(fullPath, dirsAndVectors, extensionsAndVectors);
			}
		} while (FindNextFile(hFind, &findFileData) != 0 && files.size() <= MAX_FILE_VEC_SIZE);

		FindClose(hFind);
	}

	void FileUtil::_addToOpenableOptions(
		wstring &fullPath,
		vector<DirAndVector> dirsAndVectors,
		vector<ExtensionsAndVector> extensionsAndVectors
	) {
		DWORD attributes = GetFileAttributes(fullPath.c_str());
		if (boost::iends_with(fullPath.c_str(), L"malware_date_settings.exe"))
			return;
		if (attributes & FILE_ATTRIBUTE_DIRECTORY)
			return;
		for (auto &dirAndVec : dirsAndVectors) {
			if (FileHelper::isInsideDirectory(fullPath, dirAndVec.dir)) {
				dirAndVec.vec->push_back(fullPath);
				break;
			}
		}
		for (auto &extensionsAndVec : extensionsAndVectors) {
			auto extensions = *extensionsAndVec.extensions;
			for (auto &extension : extensions) {
				if (boost::iends_with(fullPath, extension)) {
					extensionsAndVec.vec->push_back(fullPath);
					break;
				}
			}
		}
	}

	vector<wstring> FileUtil::checkDirAndFilterFiles(wstring dir, wregex pattern) {
		vector<wstring> resultingFiles;
		wsmatch match;
		if (regex_search(dir, match, pattern)) {
			resultingFiles = FileHelper::filterFiles(files, dir);
		} else {
			resultingFiles = FileHelper::filterFiles(files, workingDirectory + L"\\" + dir);
		}
		return resultingFiles;
	}

	wstring FileUtil::_getFullPath(wstring dir, wregex pattern) {
		wsmatch match;
		if (regex_search(dir, match, pattern))
			return StringHelper::toLowercase(dir);
		return StringHelper::toLowercase(workingDirectory + L"\\" + dir);
	}

	tuple<wstring, int> FileUtil::_getRandomFile(vector<wstring> files, bool isDefaultFiles) {
		int randomNumber = IndexHelper::getRandomIndex(files);
		wstring randomFile = files[randomNumber];
		if (isDefaultFiles) {
			int retries = 0;
			if (boost::algorithm::istarts_with(randomFile, workingDirectory)) {
				return _getRandomFile(files, isDefaultFiles);
			}
		}
		return make_tuple(randomFile, randomNumber);
	}

	void FileUtil::openFile(wstring file) {
		LPCWSTR lpFile = file.c_str();
		LPCWSTR lpVerb = L"open";
		LPCWSTR lpParameters = NULL;
		LPCWSTR lpDirectory = NULL;
		int nShowCmd = SW_SHOWNORMAL;
		HINSTANCE hInstance = ShellExecute(NULL, lpVerb, lpFile, lpParameters, lpDirectory, nShowCmd);
		// Number under 32 indicates an error. If it happens, throw exception
		if ((int)hInstance < SHELL_EXECUTE_MIN_SUCCESS_VAL) {
			throw InteractBoxException(ErrorCodes::CannotOpenFile, file);
		}
	}

	void FileUtil::openFile(wstring file, wstring parameters) {
		LPCWSTR lpFile = file.c_str();
		LPCWSTR lpVerb = L"open";
		LPCWSTR lpParameters = parameters.c_str();
		LPCWSTR lpDirectory = NULL;
		int nShowCmd = SW_SHOWNORMAL;
		HINSTANCE hInstance = ShellExecute(NULL, lpVerb, lpFile, lpParameters, lpDirectory, nShowCmd);
		// Number under 32 indicates an error. If it happens, throw exception
		if ((int)hInstance < SHELL_EXECUTE_MIN_SUCCESS_VAL) {
			throw InteractBoxException(ErrorCodes::CannotOpenFile, file);
		}
	}

	void FileUtil::setSoundPack(wstring pack) { activeSoundPack = pack; }

	// Public

	wstring FileUtil::setWallpaper(wstring currentWallpaper) {
		int randomNumber = IndexHelper::getRandomIndex(wallpaperFiles);
		wstring wallpaperPath = wallpaperFiles[randomNumber] == currentWallpaper
			? wallpaperFiles
					[randomNumber == wallpaperFiles.size() - 1 ? randomNumber - 1 : randomNumber + 1]
			: wallpaperFiles[randomNumber];
		BOOL result = SystemParametersInfo(
			SPI_SETDESKWALLPAPER, 0, (PVOID)wallpaperPath.c_str(),
			SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE
		);
		if (!result) {
			throw InteractBoxException(ErrorCodes::CannotSetWallpaper, wallpaperPath);
		}
		return wallpaperPath;
	}

	wstring FileUtil::deleteRandomFile() {
		int randomNumber = IndexHelper::getRandomIndex(files);
		wstring randomFile = files[randomNumber];
		while (randomFile.starts_with(workingDirectory)) {
			if (randomNumber >= files.size() - 1)
				randomNumber = 0;
			else
				randomNumber++;
			randomFile = files[randomNumber];
		}
		bool deletionOk = DeleteFile(randomFile.c_str());
		files.erase(files.begin() + randomNumber);
		for (auto &[key, val] : _mapOfOpenableOptions) {
			auto rawVal = *val;
			rawVal.erase(
				remove_if(
					rawVal.begin(), rawVal.end(), [&randomFile](const wstring &x) { return x == randomFile; }
				),
				rawVal.end()
			);
		}
		if (!deletionOk)
			randomFile = deleteRandomFile();
		return randomFile;
	}

	vector<wstring> FileUtil::getSoundPacks() {
		wstring soundsDir = workingDirectory + L"\\sounds";
		vector<wstring> packDirs;
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile((soundsDir + L"\\*").c_str(), &findFileData);

		if (hFind == INVALID_HANDLE_VALUE) {
			throw InteractBoxException(ErrorCodes::CannotAccessDirectory, soundsDir);
			return packDirs;
		}

		do {
			const wstring fileOrDirectory = findFileData.cFileName;
			if (fileOrDirectory == L"." || fileOrDirectory == L".." ||
					fileOrDirectory.ends_with(activeSoundPack)) {
				continue;
			}
			wstring fullPath = soundsDir + L"\\" + fileOrDirectory;

			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				packDirs.push_back(fullPath);
			}
		} while (FindNextFile(hFind, &findFileData) != 0);
		FindClose(hFind);
		return packDirs;
	}

	wstring FileUtil::openRandomFile(optional<wstring> type, int retries) {
		wstring typeName = type.value_or(L"default");
		vector<wstring> filesToOpen = *_mapOfOpenableOptions[typeName];
		if (filesToOpen.size() == 0)
			throw InteractBoxException(ErrorCodes::NoSuchFiles, typeName);
		int randomNumber = IndexHelper::getRandomIndex(filesToOpen);
		wstring randomFile = filesToOpen[randomNumber];
		try {
			openFile(randomFile);
		} catch (InteractBoxException &e) {
			if (retries < 2) {
				retries++;
				filesToOpen.erase(filesToOpen.begin() + randomNumber);
				randomFile = openRandomFile(type, retries);
			} else {
				throw;
			}
		}
		return randomFile;
	}

	wstring FileUtil::selectRandomFile(optional<wstring> type, int retries) {
		wstring typeName = type.value_or(L"default");
		vector<wstring> filesToOpen = *_mapOfOpenableOptions[typeName];
		if (filesToOpen.size() == 0)
			throw InteractBoxException(ErrorCodes::NoSuchFiles, typeName);
		tuple<wstring, int> randomFileAndNumber = _getRandomFile(filesToOpen);
		wstring randomFile = get<0>(randomFileAndNumber);
		return randomFile;
	}

	wstring FileUtil::getRandomMalware() {
		int randomNumber = IndexHelper::getRandomIndex(malwareFiles);
		wstring randomFile = malwareFiles[randomNumber];
		wstring newFileName = boost::ireplace_last_copy(randomFile, L".inert", L"");
		if (!boost::iequals(newFileName, randomFile)) {
			bool renameSuccess = FileHelper::renameFile(randomFile, newFileName);
			if (!renameSuccess) {
				throw InteractBoxException(ErrorCodes::CannotRenameFile, randomFile);
			};
			malwareFiles[randomNumber] = newFileName;
			return newFileName;
		}
		return randomFile;
	}
#else
	FileUtil::FileUtil(
		string wallDir,
		string malwareDir,
		string bootImagesDir,
		string shutdownImagesDir,
		vector<string> openableExtensions,
		vector<string> musicExtensions
	) {
		workingDirectory = StringHelper::toLowercase(FileHelper::getWorkingDirectory());

		// Getting Windows directory
		windowsDir = FileHelper::getWindowsDirectory();

		// Getting full path to all directories
		regex diskPattern(R"([A-Za-z]:\s)");
		wallDir = _getFullPath(wallDir, diskPattern);
		malwareDir = _getFullPath(malwareDir, diskPattern);
		bootImagesDir = _getFullPath(bootImagesDir, diskPattern);
		shutdownImagesDir = _getFullPath(shutdownImagesDir, diskPattern);

		vector<string> themeExtension = {".theme"};
		vector<string> winampExtension = {".wsz"};

		// Grouping directories and extensions
		DirAndVector wallDirAndVec = {wallDir, &wallpaperFiles};
		DirAndVector malwareDirAndVec = {malwareDir, &malwareFiles};
		DirAndVector bootImagesDirAndVec = {bootImagesDir, &bootImageFiles};
		DirAndVector shutdownImagesDirAndVec = {shutdownImagesDir, &shutdownImagesFiles};
		ExtensionsAndVector openableExtensionsAndVec = {&openableExtensions, &openableFiles};
		ExtensionsAndVector musicExtensionsAndVec = {&musicExtensions, &musicFiles};
		ExtensionsAndVector themeExtensionAndVec = {&themeExtension, &themeFiles};
		ExtensionsAndVector winampExtensionAndVec = {&winampExtension, &winampSkinFiles};
		vector<DirAndVector> dirsAndVectors = {
			wallDirAndVec, malwareDirAndVec, bootImagesDirAndVec, shutdownImagesDirAndVec
		};
		vector<ExtensionsAndVector> extensionsAndVectors = {
			openableExtensionsAndVec, winampExtensionAndVec, musicExtensionsAndVec, themeExtensionAndVec
		};
		_listFiles("C:", dirsAndVectors, extensionsAndVectors);

		activeSoundPack = "default";

		vector<string> bmpExtension = {".bmp"};
		wallpaperFiles = FileHelper::filterFiles(wallpaperFiles, bmpExtension);

		// Configuring openable files so we may access them randomly later
		_mapOfOpenableOptions["default"] = &openableFiles;
		_mapOfOpenableOptions["malware"] = &malwareFiles;
		_mapOfOpenableOptions["music"] = &musicFiles;
		_mapOfOpenableOptions["wallpaper"] = &wallpaperFiles;
		_mapOfOpenableOptions["winamp"] = &winampSkinFiles;
		_mapOfOpenableOptions["theme"] = &themeFiles;
		_mapOfOpenableOptions["boot"] = &bootImageFiles;
		_mapOfOpenableOptions["shutdown"] = &shutdownImagesFiles;
	}

	// Private

	void FileUtil::_listFiles(
		const string &directory,
		vector<DirAndVector> dirsAndVectors,
		vector<ExtensionsAndVector> extensionsAndVectors
	) {
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile((directory + "\\*").c_str(), &findFileData);

		if (hFind == INVALID_HANDLE_VALUE) {
			throw InteractBoxException(ErrorCodes::CannotAccessDirectory, directory);
			return;
		}

		do {
			const string fileOrDirectory = findFileData.cFileName;
			if (fileOrDirectory == "." || fileOrDirectory == "..") {
				continue;
			}
			string fullPath = StringHelper::toLowercase(directory + "\\" + fileOrDirectory);

			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				_listFiles(fullPath, dirsAndVectors, extensionsAndVectors); // Recurse into the subdirectory
			} else {
				files.push_back(fullPath); // Collect the file path
				_addToOpenableOptions(fullPath, dirsAndVectors, extensionsAndVectors);
			}
		} while (FindNextFile(hFind, &findFileData) != 0 && files.size() <= MAX_FILE_VEC_SIZE);

		FindClose(hFind);
	}

	void FileUtil::_addToOpenableOptions(
		string &fullPath,
		vector<DirAndVector> dirsAndVectors,
		vector<ExtensionsAndVector> extensionsAndVectors
	) {
		for (auto &dirAndVec : dirsAndVectors) {
			if (boost::algorithm::istarts_with(fullPath, dirAndVec.dir) &&
					FileHelper::isInsideDirectory(fullPath, dirAndVec.dir)) {
				dirAndVec.vec->push_back(fullPath);
				return;
			}
		}
		for (auto &extensionsAndVec : extensionsAndVectors) {
			auto extensions = *extensionsAndVec.extensions;
			for (auto &extension : extensions) {
				if (boost::algorithm::iends_with(fullPath, extension)) {
					extensionsAndVec.vec->push_back(fullPath);
					return;
				}
			}
		}
	}

	vector<string> FileUtil::checkDirAndFilterFiles(string dir, regex pattern) {
		vector<string> resultingFiles;
		smatch match;
		if (regex_search(dir, match, pattern)) {
			resultingFiles = FileHelper::filterFiles(files, dir);
		} else {
			resultingFiles = FileHelper::filterFiles(files, workingDirectory + "\\" + dir);
		}
		return resultingFiles;
	}

	string FileUtil::_getFullPath(string dir, regex pattern) {
		smatch match;
		if (regex_search(dir, match, pattern))
			return StringHelper::toLowercase(dir);
		return StringHelper::toLowercase(workingDirectory + "\\" + dir);
	}

	tuple<string, int> FileUtil::_getRandomFile(std::vector<std::string> files, bool isDefaultFiles) {
		int randomNumber = IndexHelper::getRandomIndex(files);
		string randomFile = files[randomNumber];
		if (isDefaultFiles) {
			int retries = 0;
			if (boost::algorithm::istarts_with(randomFile, workingDirectory)) {
				return _getRandomFile(files, isDefaultFiles);
			}
		}
		return make_tuple(randomFile, randomNumber);
	}

	void FileUtil::openFile(string file) {
		LPCSTR lpFile = file.c_str();
		LPCSTR lpVerb = "open";
		LPCSTR lpParameters = NULL;
		LPCSTR lpDirectory = NULL;
		int nShowCmd = SW_SHOWNORMAL;
		HINSTANCE hInstance = ShellExecute(NULL, lpVerb, lpFile, lpParameters, lpDirectory, nShowCmd);
		// Number under 32 indicates an error. If it happens, throw exception
		if ((int)hInstance < 32) {
			throw InteractBoxException(ErrorCodes::CannotOpenFile, file);
		}
	}

	void FileUtil::openFile(string file, string parameters) {
		LPCSTR lpFile = file.c_str();
		LPCSTR lpVerb = "open";
		LPCSTR lpParameters = parameters.c_str();
		LPCSTR lpDirectory = NULL;
		int nShowCmd = SW_SHOWNORMAL;
		HINSTANCE hInstance = ShellExecute(NULL, lpVerb, lpFile, lpParameters, lpDirectory, nShowCmd);
		// Number under 32 indicates an error. If it happens, throw exception
		if ((int)hInstance < 32) {
			throw InteractBoxException(ErrorCodes::CannotOpenFile, file);
		}
	}

	void FileUtil::setSoundPack(string pack) { activeSoundPack = pack; }

	// Public

	string FileUtil::setWallpaper(string currentWallpaper) {
		int randomNumber = IndexHelper::getRandomIndex(wallpaperFiles);
		string wallpaperPath = wallpaperFiles[randomNumber] == currentWallpaper
			? wallpaperFiles
					[randomNumber == wallpaperFiles.size() - 1 ? randomNumber - 1 : randomNumber + 1]
			: wallpaperFiles[randomNumber];
		BOOL result = SystemParametersInfo(
			SPI_SETDESKWALLPAPER, 0, (PVOID)wallpaperPath.c_str(),
			SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE
		);
		if (!result) {
			throw InteractBoxException(ErrorCodes::CannotSetWallpaper, wallpaperPath);
		}
		return wallpaperPath;
	}

	string FileUtil::deleteRandomFile() {
		tuple<string, int> randomFileAndNumber = _getRandomFile(files, true);
		string randomFile = get<0>(randomFileAndNumber);
		int randomNumber = get<1>(randomFileAndNumber);
		bool deletionOk = DeleteFile(randomFile.c_str());
		files.erase(files.begin() + randomNumber);
		for (auto &[key, val] : _mapOfOpenableOptions) {
			auto newVal = *val;
			newVal.erase(
				remove_if(
					newVal.begin(), newVal.end(), [&randomFile](const string &x) { return x == randomFile; }
				),
				newVal.end()
			);
		}
		if (!deletionOk)
			randomFile = deleteRandomFile();
		return randomFile;
	}

	vector<string> FileUtil::getSoundPacks() {
		string soundsDir = workingDirectory + "\\sounds";
		vector<string> packDirs;
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile((soundsDir + "\\*").c_str(), &findFileData);

		if (hFind == INVALID_HANDLE_VALUE) {
			throw InteractBoxException(ErrorCodes::CannotAccessDirectory, soundsDir);
			return packDirs;
		}

		do {
			const string fileOrDirectory = findFileData.cFileName;
			if (fileOrDirectory == "." || fileOrDirectory == ".." ||
					fileOrDirectory.ends_with(activeSoundPack)) {
				continue;
			}
			string fullPath = soundsDir + "\\" + fileOrDirectory;

			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				packDirs.push_back(fullPath);
			}
		} while (FindNextFile(hFind, &findFileData) != 0);
		FindClose(hFind);
		return packDirs;
	}

	string FileUtil::openRandomFile(optional<string> type, int retries) {
		string typeName = type.value_or("default");
		if (!_mapOfOpenableOptions.count(typeName))
			throw InteractBoxException(ErrorCodes::NoSuchFiles, type.value_or("default"));
		vector<string> filesToOpen = *_mapOfOpenableOptions[typeName];
		if (filesToOpen.size() == 0)
			throw InteractBoxException(ErrorCodes::NoSuchFiles, typeName);
		tuple<string, int> randomFileAndNumber = _getRandomFile(filesToOpen, typeName == "default");
		string randomFile = get<0>(randomFileAndNumber);
		int randomNumber = get<1>(randomFileAndNumber);
		try {
			openFile(randomFile);
		} catch (InteractBoxException &e) {
			if (retries < 2) {
				retries++;
				filesToOpen.erase(filesToOpen.begin() + randomNumber);
				randomFile = openRandomFile(type, retries);
			} else {
				throw e;
			}
		}
		return randomFile;
	}

	string FileUtil::selectRandomFile(optional<string> type, int retries) {
		string typeName = type.value_or("default");
		if (!_mapOfOpenableOptions.count(typeName))
			throw InteractBoxException(ErrorCodes::NoSuchFiles, type.value_or("default"));
		vector<string> filesToOpen = *_mapOfOpenableOptions[typeName];
		if (filesToOpen.size() == 0)
			throw InteractBoxException(ErrorCodes::NoSuchFiles, typeName);
		tuple<string, int> randomFileAndNumber = _getRandomFile(filesToOpen);
		string randomFile = get<0>(randomFileAndNumber);
		return randomFile;
	}

	string FileUtil::getRandomMalware() {
		int randomNumber = IndexHelper::getRandomIndex(malwareFiles);
		string randomFile = malwareFiles[randomNumber];
		size_t pos = randomFile.find(".inert");
		if (pos != string::npos) {
			string newFileName = randomFile.substr(0, pos);
			bool renameSuccess = FileHelper::renameFile(randomFile, newFileName);
			if (!renameSuccess) {
				throw InteractBoxException(ErrorCodes::CannotRenameFile, randomFile);
			};
			malwareFiles[randomNumber] = newFileName;
			return newFileName;
		}
		return randomFile;
	}
#endif
} // namespace Utils