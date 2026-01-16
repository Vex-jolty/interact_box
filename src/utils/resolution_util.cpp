#include "utils/resolution_util.hpp"

namespace Utils {
	using namespace std;
	/** cSpell:disable */
	void ResolutionUtil::changeResolution(bool toMaximum) {
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
			throw InteractBoxException(ErrorCodes::CannotEnumerateDisplaySettings);
		}
		devMode.dmPelsWidth = finalWidth;
		devMode.dmPelsHeight = finalHeight;
		devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		LONG result = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
		if (result != DISP_CHANGE_SUCCESSFUL) {
			throw InteractBoxException(ErrorCodes::CannotChangeDisplay);
		}
	}

	void ResolutionUtil::changeColors(int bitsPerPixel) {
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
			throw InteractBoxException(ErrorCodes::InvalidColorSetting, to_string(bitsPerPixel));
		}
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmBitsPerPel = bitsPerPixel;
		devMode.dmFields = DM_BITSPERPEL;
		LONG result = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
		if (result != DISP_CHANGE_SUCCESSFUL) {
			throw InteractBoxException(ErrorCodes::CannotChangeDisplay);
		}
	}
} // namespace Utils