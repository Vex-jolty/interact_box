#include "utils/time_util.hpp"

namespace Utils {
	using namespace std;
	string TimeUtil::getAndFormatCurrentTime(string format) {
		time_t now = time(0);
		char buffer[90];
		struct tm* timeInfo = localtime(&now);
		strftime(buffer, sizeof(buffer), format.c_str(), timeInfo);
		return buffer;
	}

	void TimeUtil::setSystemDateTime(int year, int month, int day) {
		SYSTEMTIME sysTime;
		ZeroMemory(&sysTime, sizeof(sysTime));
		sysTime.wYear = year;
		sysTime.wMonth = month;
		sysTime.wDay = day;
		sysTime.wHour = 12;
		sysTime.wMinute = 0;
		sysTime.wSecond = 1;
		WINBOOL success = SetSystemTime(&sysTime);
		if (!success)
			throw InteractBoxException(ErrorCodes::CannotSetSystemDateTime);
	}
} // namespace Utils