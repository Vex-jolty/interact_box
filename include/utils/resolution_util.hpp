#pragma once
#include "utils.hpp"

namespace Utils {
	class ResolutionUtil {
		public:
			static void changeResolution(bool toMaximum);
			static void changeColors(int bitsPerPixel);
	};
} // namespace Utils
