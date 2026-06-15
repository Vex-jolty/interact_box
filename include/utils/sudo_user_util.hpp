#pragma once
#include <string>
#include <unistd.h>
#include <pwd.h>

namespace Utils {
	class SudoUserUtil {
		public:
			SudoUserUtil();
			const std::string& getRealUser();
		private:
			std::string _realUser;
			const std::string _setRealUser();
	};
} // namespace Utils
