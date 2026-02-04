#pragma once
#include <ctime>
#ifdef WIN32
	#include <winsock2.h>
	#include <windows.h>
#endif
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <map>
#ifdef WIN32
	#include <tlhelp32.h>
	#include <tchar.h>
#endif
#include <optional>
#include "exported.hpp"
#include <regex>