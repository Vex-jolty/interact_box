#include "errors/error_handler.hpp"


namespace Errors {
	using namespace std;
	ErrorHandler::ErrorHandler(
		shared_ptr<Utils::LoggingUtil> loggingUtil,
		#if WINVER > _WIN32_WINNT_NT4
		wstring messageBoxPath
		#else
		string messageBoxPath
		#endif
	) {
		_loggingUtil = loggingUtil;
		_messageBoxPath = messageBoxPath;
	}

	void ErrorHandler::handleError(HttpStatus::Code& c, string& route) {
		_loggingUtil->err("Route " + route + " raised status " + to_string(c) + " " + HttpStatus::reasonPhrase(c));
  }

	#if WINVER > _WIN32_WINNT_NT4
	void ErrorHandler::handleError(wstring& e) {
		string message = StringHelper::wideStringToString(e);
		_loggingUtil->err(message);
		showErrorBox(e);
	}

	void ErrorHandler::handleError(wstring* e) {
		string message = StringHelper::wideStringToString(*e);
		_loggingUtil->err(message);
		showErrorBox(*e);
	}
	#endif
	
	void ErrorHandler::handleError(string& e) {
		_loggingUtil->err(e);
		showErrorBox(e);
	}

	void ErrorHandler::handleError(string* e) {
		_loggingUtil->err(*e);
		showErrorBox(*e);
	}

	void ErrorHandler::handleError(exception& e) {
		string message = e.what();
		_loggingUtil->err(message);
		showErrorBox(message);
	}

	void ErrorHandler::handleError(exception* e) {
		string message = e->what();
		_loggingUtil->err(message);
		showErrorBox(message);
	}

	void ErrorHandler::handleError(InteractBoxException& e) {
		string message = e.what();
		_loggingUtil->err(message);
		showErrorBox(message);
	}

	void ErrorHandler::handleError(InteractBoxException* e) {
		string message = e->what();
		_loggingUtil->err(message);
		showErrorBox(message);
	}
	
	#if WINVER > _WIN32_WINNT_NT4
  void ErrorHandler::showErrorBox(string message) {
		Utils::MessageBoxUtil::createBox(L"INTERACT BOX ERROR", StringHelper::stringToWideString(message), L"e", L"ok");
  }

	void ErrorHandler::showErrorBox(wstring message) {
		Utils::MessageBoxUtil::createBox(L"INTERACT BOX ERROR", message, L"e", L"ok");
  }
	#else
	void ErrorHandler::showErrorBox(string message) {
		Utils::MessageBoxUtil::createBox("INTERACT BOX ERROR", message, "e", "ok");
  }
	#endif
}