#include "utils/resolution_util.hpp"

namespace Utils {
	using namespace std;
	/** cSpell:disable */
	void ResolutionUtil::changeResolution(bool toMaximum) {
#ifdef WIN32
		DEVMODE devMode;
		ZeroMemory(&devMode, sizeof(DEVMODE));
		devMode.dmSize = sizeof(DEVMODE);
		int finalWidth = 1000;
		int finalHeight = 1000;
		for (int i = 0; EnumDisplaySettings(NULL, i, &devMode) != 0; i++) {
			if (toMaximum) {
				if (devMode.dmBitsPerPel < 32)
					continue;
				if (finalWidth < devMode.dmPelsWidth)
					finalWidth = devMode.dmPelsWidth;
				if (finalHeight < devMode.dmPelsHeight)
					finalHeight = devMode.dmPelsHeight;
			} else {
				if (finalWidth > devMode.dmPelsWidth)
					finalWidth = devMode.dmPelsWidth;
				if (finalHeight > devMode.dmPelsHeight)
					finalHeight = devMode.dmPelsHeight;
			}
		}
		if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode) == 0) {
			throw InteractBoxException(ErrorCodes::ErrorCode::CannotEnumerateDisplaySettings);
		}
		devMode.dmPelsWidth = finalWidth;
		devMode.dmPelsHeight = finalHeight;
		devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		LONG result = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
		if (result != DISP_CHANGE_SUCCESSFUL) {
			throw InteractBoxException(ErrorCodes::ErrorCode::CannotChangeDisplay);
		}
#else
		array<char, 4096> buffer;
		string xrandrResult;
		unique_ptr<FILE, decltype(&pclose)> pipe(popen("xrandr", "r"), pclose);
		if (!pipe) {
			throw InteractBoxException(ErrorCodes::ErrorCode::CannotOpenProcess, "xrandr");
		}
		while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
			xrandrResult += buffer.data();
		}
		vector<string> xrandrLines;
		boost::split(xrandrLines, xrandrResult, boost::is_any_of("\n"));
		regex resolutionPattern(R"(^\s+(\d+x\d+)\s+(\d+\.\d+))");
		vector<ScreenInfo> screens;
		int currentIndex = -1;
		for (auto& line : xrandrLines) {
			if (boost::istarts_with(line, "screen 0"))
				continue;
			if (boost::iends_with(line, "mm")) {
				vector<string> splitLine;
				boost::split(splitLine, line, boost::is_any_of(" "));

				const string currentScreen = splitLine[0];
				screens.push_back(ScreenInfo{currentScreen, "", ""});
				currentIndex++;
			}
			smatch match;
			regex_search(line, match, resolutionPattern);
			if (match.size() == 0)
				continue;

			string resolution = match[1];
			string framerate = match[2];

			if (screens[currentIndex].resolution.empty()) {
				screens[currentIndex].resolution = match[1];
				screens[currentIndex].framerate = match[2];
				continue;
			}

			vector<string> splitCurrentRes;
			vector<string> splitRes;

			boost::split(splitCurrentRes, screens[currentIndex].resolution, boost::is_any_of("x"));
			boost::split(splitRes, resolution, boost::is_any_of("x"));

			bool newResIsBigger = stoi(splitRes[0]) > stoi(splitCurrentRes[0]);
			if ((toMaximum && newResIsBigger) || (!toMaximum && !newResIsBigger)) {
				screens[currentIndex].resolution = match[1];
				screens[currentIndex].framerate = match[2];
			}
		}

		for (auto& item : screens) {
			int result = system(("xrandr --output " + item.name + " --mode " + item.resolution +
													 " --rate " + item.framerate)
														.c_str());
			if (result != 0)
				throw InteractBoxException(ErrorCodes::ErrorCode::CannotChangeDisplay);
		}
#endif
	}

	void ResolutionUtil::changeColors(int bitsPerPixel) {
#ifdef WIN32
		DEVMODE devMode;
		ZeroMemory(&devMode, sizeof(DEVMODE));
		vector<int> validValues = {
			4,	// 16 colors
			8,	// 256 colors
			16, // 16 bits
			24, // 24 bits
			32, // 32 bits
		};
		if (find(validValues.begin(), validValues.end(), bitsPerPixel) == validValues.end()) {
			throw InteractBoxException(
				ErrorCodes::ErrorCode::InvalidColorSetting, to_string(bitsPerPixel)
			);
		}
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmBitsPerPel = bitsPerPixel;
		devMode.dmFields = DM_BITSPERPEL;
		LONG result = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
		if (result != DISP_CHANGE_SUCCESSFUL) {
			throw InteractBoxException(ErrorCodes::ErrorCode::CannotChangeDisplay);
		}
#endif
	}
} // namespace Utils