#include "utils/sudo_user_util.hpp"

namespace Utils {
	SudoUserUtil::SudoUserUtil() {
		_realUser = _setRealUser();
	}

	const std::string SudoUserUtil::_setRealUser() {
		const char* sudoUser = std::getenv("SUDO_USER");
		if (sudoUser) {
			return sudoUser;
		}
		uid_t uid = getuid();
		struct passwd* pw = getpwuid(uid);
		if (pw) {
			return std::string(pw->pw_name);
		}
		return "";
	}

	const std::string& SudoUserUtil::getRealUser() {
		return _realUser;
	}
} // namespace Utils
