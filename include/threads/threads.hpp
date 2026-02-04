#pragma once
#include <thread>
#include <pthread.h>
#ifdef WIN32
	#include <winsock2.h>
	#include <windows.h>
#endif
#include "server/web_server.hpp"
#include "errors/error_handler.hpp"
