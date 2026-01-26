#pragma once
#ifdef WIN32
	#ifndef _WINSOCK2API_
		#include <winsock2.h>
	#endif
	#ifndef _WINDOWS_
		#include <windows.h>
	#endif
#endif
#ifndef _GLIBCXX_STRING
	#include <string>
#endif
#ifndef _GLIBCXX_IOSTREAM
	#include <iostream>
#endif
#include "utils/logging_util.hpp"
#include "utils/shell_util.hpp"
#include "utils/message_box_util.hpp"
#include "exported.hpp"