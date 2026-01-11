#pragma once
#include "utils.hpp"

namespace Utils {
	class TimeUtil {
		public:
			static std::string getAndFormatCurrentTime(std::string format);
			static void setSystemDateTime(int year, int month, int day);
	};
} // namespace Utils
