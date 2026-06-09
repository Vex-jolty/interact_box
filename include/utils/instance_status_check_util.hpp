#pragma once
#ifdef WIN32
	#include "utils.hpp"
	#include <cstdint>
	#include "general_definitions.hpp"
	#include "errors/interact_box_exceptions.hpp"
namespace Utils {
	class InstanceStatusCheckUtil {
		public:
			static void checkStatus(HINSTANCE instance, const InteractBoxException& e);
	};
} // namespace Utils

#endif