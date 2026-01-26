#pragma once
#include <ctime>
#ifdef WIN32
	#ifndef _WINSOCK2API_
		#include <winsock2.h>
	#endif
	#ifndef _WINDOWS_
		#include <windows.h>
	#endif
#endif
#ifndef _GLIBCXX_VECTOR
	#include <vector>
#endif
#ifndef _GLIBCXX_STRING
	#include <string>
#endif
#ifndef _GLIBCXX_IOSTREAM
	#include <iostream>
#endif
#ifndef _GLIBCXX_ALGORITHM
	#include <algorithm>
#endif
#ifndef _GLIBCXX_ITERATOR
	#include <iterator>
#endif
#ifndef _GLIBCXX_FSTREAM
	#include <fstream>
#endif
#ifndef _GLIBCXX_MAP
	#include <map>
#endif
#ifdef WIN32
	#ifndef _INC_TOOLHELP32
		#include <tlhelp32.h>
	#endif
	#ifndef _INC_TCHAR
		#include <tchar.h>
	#endif
#endif
#ifndef _GLIBCXX_OPTIONAL
	#include <optional>
#endif
#include "exported.hpp"
#include <regex>