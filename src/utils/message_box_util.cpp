#include "utils/message_box_util.hpp"

namespace Utils {
	using namespace std;
#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
	void MessageBoxUtil::createBox(wstring title, wstring content, wstring type, wstring buttons) {
		wstring processName = L"message_box_process.exe";
		wstring workingDir = FileHelper::getWorkingDirectory();
		ShellUtil::openShell(
			processName, L"open", workingDir,
			L"--title " + getParsedString(title) + L" --content " + getParsedString(content) +
				L" --type " + getParsedString(type) + L" --buttons " + getParsedString(buttons)
		);
		ProcessHelper::setToForeground(workingDir + L"\\" + processName);
	}

	wstring MessageBoxUtil::getParsedString(wstring input) {
		wstring quotes = L"\"";
		if (input.find(L' ') == wstring::npos ||
				(input.starts_with(quotes) && input.ends_with(quotes))) {
			return input;
		}
		return quotes + input + quotes;
	}
#else
	void MessageBoxUtil::createBox(string title, string content, string type, string buttons) {
		string workingDir = FileHelper::getWorkingDirectory();
	#ifdef WIN32
		string processName = "message_box_process.exe";
		ShellUtil::openShell(
			processName, "open", workingDir,
			"--title " + getParsedString(title) + " --content " + getParsedString(content) + " --type " +
				getParsedString(type) + " --buttons " + getParsedString(buttons)
		);
		ProcessHelper::setToForeground(workingDir + "\\" + processName);
	#else
		string processName = "message_box_process";
		string path = workingDir + "/" + processName;
		string args = " --title " + getParsedString(title) + " --content " + getParsedString(content) +
			" --type " + getParsedString(type) + " --buttons \"" + getParsedString(buttons) + "\"";
		system((path + args + " &").c_str());
	#endif
	}

	string MessageBoxUtil::getParsedString(string input) {
		char quotes = '"';
		if (input.find(' ') == string::npos || (input.starts_with(quotes) && input.ends_with(quotes))) {
			return input;
		}
		return quotes + input + quotes;
	}
#endif
} // namespace Utils