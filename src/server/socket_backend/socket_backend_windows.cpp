// net/socket_backend_win.cpp
#ifdef WIN32
	#include "server/socket_backend/socket_backend_windows.hpp"
	#include <cstring>

namespace Server {

	bool WinSocketBackend::init() {
		WSADATA wsa;
		return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
	}

	bool WinSocketBackend::bindAndListen(const std::string& host, int port) {
		_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (_listenSocket == INVALID_SOCKET)
			return false;

		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(host.c_str());

		if (bind(_listenSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
			return false;

		return listen(_listenSocket, SOMAXCONN) != SOCKET_ERROR;
	}

	int WinSocketBackend::acceptClient(std::string& clientIp) {
		sockaddr_in clientAddr{};
		int len = sizeof(clientAddr);

		SOCKET client = accept(_listenSocket, (sockaddr*)&clientAddr, &len);
		if (client == INVALID_SOCKET)
			return -1;

		clientIp = inet_ntoa(clientAddr.sin_addr);
		return (int)client;
	}

	int WinSocketBackend::recvData(int clientFd, std::string& out) {
		char buffer[1024];
		int received = recv((SOCKET)clientFd, buffer, sizeof(buffer) - 1, 0);
		if (received > 0) {
			buffer[received] = '\0';
			out.assign(buffer);
		}
		return received;
	}

	void WinSocketBackend::sendData(int clientFd, const std::string& data) {
		int result = send((SOCKET)clientFd, data.c_str(), (int)data.size(), 0);
    if (result != SOCKET_ERROR) return;
    throw InteractBoxException(ErrorCodes::CannotSendResponse);
	}

	void WinSocketBackend::closeClient(int clientFd) { closesocket((SOCKET)clientFd); }

	void WinSocketBackend::shutdown() {
		if (_listenSocket != INVALID_SOCKET) {
			closesocket(_listenSocket);
			_listenSocket = INVALID_SOCKET;
		}
		WSACleanup();
	}

} // namespace Server
#endif
