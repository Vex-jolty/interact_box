#include "threads/tray_thread.hpp"

/* cSpell:disable */
namespace Threads {
	using namespace Server;
	using namespace std;
	LRESULT CALLBACK TrayThread::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		WebServer* server = reinterpret_cast<WebServer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		switch (uMsg) {
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			case TRAY_MESSAGE:
				switch (lParam) {
					case WM_RBUTTONUP: {
							HMENU hMenu = CreatePopupMenu();
							if (hMenu) {
								#if WINVER > _WIN32_WINNT_NT4
								InsertMenu(hMenu, 2, MF_BYCOMMAND, 2, L"Settings");
								InsertMenu(hMenu, -1, MF_BYCOMMAND, 1, L"Exit");
								#else
								InsertMenu(hMenu, 2, MF_BYCOMMAND, 2, "Settings");
								InsertMenu(hMenu, -1, MF_BYCOMMAND, 1, "Exit");
								#endif
								POINT pt;
								GetCursorPos(&pt);
								SetForegroundWindow(hwnd);
								TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
								DestroyMenu(hMenu);
							}
							break;
						}
					case WM_MENUCOMMAND:
					case WM_COMMAND:
						if (LOWORD(wParam) == 1) {
							server->serverAbort();
							PostQuitMessage(0);
							exit(0);
						} else if (LOWORD(wParam) == 2) {
							openSettings();
						}
						break;
				}
				return 0;
			case WM_COMMAND:
				if (LOWORD(wParam) == 1) {
					server->serverAbort();
					PostQuitMessage(0);
					exit(0);
				} else if (LOWORD(wParam) == 2) {
					openSettings();
				}
				break;
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	void TrayThread::addTrayIcon(HWND hwnd) {
		NOTIFYICONDATA nid = {};
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		nid.uID = ICON_ID;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = TRAY_MESSAGE;
		#if WINVER == _WIN32_WINNT_NT4
		nid.uVersion = NOTIFYICON_VERSION;
		#endif
		nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);

		#if WINVER > _WIN32_WINNT_NT4
		wcscpy(nid.szTip, L"Interact Box");
		#else
		strcpy(nid.szTip, "Interact Box");
		#endif
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		if (!hIcon) {
			hIcon = LoadIcon(NULL, IDI_APPLICATION);
		}
		nid.hIcon = hIcon;

		Shell_NotifyIcon(NIM_ADD, &nid);
	}

#if WINVER > _WIN32_WINNT_NT4
	void TrayThread::notify(HWND hwnd, wstring message) {
		NOTIFYICONDATA nid = {};
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		nid.uID = ICON_ID;
		nid.uFlags = NIF_INFO;
		nid.uTimeout = 10;

		wcscpy(nid.szInfoTitle, L"Interact Box");
		wcscpy(nid.szInfo, message.c_str());

		nid.dwInfoFlags = NIIF_INFO;
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}
	#endif

	void TrayThread::removeTrayIcon(HWND hwnd) {
		NOTIFYICONDATA nid = {};
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		nid.uID = ICON_ID;

		Shell_NotifyIcon(NIM_DELETE, &nid);
	}

	void TrayThread::openSettings() {
		auto workingDir = FileHelper::getWorkingDirectory();
		#if WINVER > _WIN32_WINNT_NT4
		wstring settingsPath = L"interact_box_settings.exe";
		try {
			Utils::ShellUtil::openShell(settingsPath, L"open", workingDir);
			ProcessHelper::setToForeground(settingsPath);
		} catch (InteractBoxException& e) {
			Utils::MessageBoxUtil::createBox(L"ERROR", StringHelper::stringToWideString(e.what()), L"e", L"ok");
		}
		#else
		string settingsPath = "interact_box_settings.exe";
		try {
			Utils::ShellUtil::openShell(settingsPath, "open", workingDir);
			ProcessHelper::setToForeground(settingsPath);
		} catch (InteractBoxException& e) {
			Utils::MessageBoxUtil::createBox("ERROR", e.what(), "e", "ok");
		}
		#endif
	}

	void* TrayThread::trayIconThread(void* arg) {
		ThreadData* threadData = static_cast<ThreadData*>(arg);
		#if WINVER > _WIN32_WINNT_NT4
		const wchar_t CLASS_NAME[] = L"HiddenWindowClass";
		#else
		const char CLASS_NAME[] = "HiddenWindowClass";
		#endif
		WNDCLASS wc = {};

		wc.lpfnWndProc = windowProc;
		wc.hInstance = threadData->hInstance;
		wc.lpszClassName = CLASS_NAME;

		RegisterClass(&wc);

		HWND hwnd = CreateWindowEx(
				0,
				CLASS_NAME,
				#if WINVER > _WIN32_WINNT_NT4
				L"Hidden Window",
				#else
				"Hidden Window",
				#endif
				0,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				NULL,
				NULL,
				threadData->hInstance,
				NULL
		);

		if (hwnd == NULL) {
			return NULL;
		}
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(threadData->server));
		addTrayIcon(hwnd);
		#if WINVER > _WIN32_WINNT_NT4
		notify(hwnd, L"Interact Box is now online!");
		#else
		Utils::MessageBoxUtil::createBox("Interact Box", "Interact Box is now online", "i", "ok");
		#endif
		MSG msg = {};
		while (!(threadData->server->abortNow.load()) && GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		removeTrayIcon(hwnd);

		return NULL;
	}
}