#ifdef WIN32
	#include "utils/instance_status_check_util.hpp"

namespace Utils {
	void InstanceStatusCheckUtil::checkStatus(HINSTANCE instance, const InteractBoxException& e) {
	#if INTPTR_MAX == INT32_MAX
		if ((int)instance < SHELL_EXECUTE_MIN_SUCCESS_VAL) {
	#else
		if ((long long)instance < SHELL_EXECUTE_MIN_SUCCESS_VAL) {
	#endif
			throw e;
		}
	}
} // namespace Utils
#endif