#include "threads/threads.hpp"
#include "exported.hpp"
#include "utils/shell_util.hpp"
#include "resources.h"
#include <shellapi.h>
#define TRAY_MESSAGE (WM_USER + 1)
#define ICON_ID 1001

namespace Threads {
	struct ThreadData {
		HINSTANCE hInstance;
		Server::WebServer *server;
		HWND *hwndPtr;
	};
	class TrayThread {
		public:
			static void *trayIconThread(void *arg);

		private:
			static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			static void addTrayIcon(HWND hwnd);
#if WINVER > _WIN32_WINNT_NT4
			static void notify(HWND hwnd, std::wstring message);
#endif
			static void removeTrayIcon(HWND hwnd);
			static void openSettings();
	};
} // namespace Threads
