#pragma once
#include "utils.hpp"

namespace Utils {
#ifdef __linux__
	struct ScreenInfo {
		public:
			const std::string name;
			std::string resolution;
			std::string framerate;
	};
#endif
	class ResolutionUtil {
		public:
			static void changeResolution(bool toMaximum);
			static void changeColors(int bitsPerPixel);
	};
} // namespace Utils
