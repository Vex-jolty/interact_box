#pragma once
#include <thread>
#include <pthread.h>
#ifndef _WINSOCK2API_
	#include <winsock2.h>
#endif
#ifndef _WINDOWS_
	#include <windows.h>
#endif
#include "server/web_server.hpp"
#include "errors/error_handler.hpp"
