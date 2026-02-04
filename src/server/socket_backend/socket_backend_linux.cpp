// net/socket_backend_linux.cpp
#ifdef __linux__
	#include "server/socket_backend/socket_backend_linux.hpp"
	#include <cstring>

namespace Server {
	bool LinuxSocketBackend::bindAndListen(const std::string& host, int port) {
		_listenFd = socket(AF_INET, SOCK_STREAM, 0);
		if (_listenFd < 0)
			return false;

		int opt = 1;
		setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(host.c_str());

		if (bind(_listenFd, (sockaddr*)&addr, sizeof(addr)) < 0)
			return false;

		return listen(_listenFd, SOMAXCONN) == 0;
	}

	int LinuxSocketBackend::acceptClient(std::string& clientIp) {
		sockaddr_in clientAddr{};
		socklen_t len = sizeof(clientAddr);

		int client = accept(_listenFd, (sockaddr*)&clientAddr, &len);
		if (client < 0)
			return -1;

		clientIp = inet_ntoa(clientAddr.sin_addr);
		return client;
	}

	int LinuxSocketBackend::recvData(int clientFd, std::string& out) {
		char buffer[HTTP_READ_BUFFER_SIZE];
		int received = recv(clientFd, buffer, sizeof(buffer), 0);
		if (received > 0) {
			buffer[received] = '\0';
			out.assign(buffer);
		}
		return received;
	}

	void LinuxSocketBackend::sendData(int clientFd, const std::string& data) {
		send(clientFd, data.c_str(), data.size(), 0);
	}

	void LinuxSocketBackend::closeClient(int clientFd) { close(clientFd); }

	void LinuxSocketBackend::shutdown() {
		if (_listenFd >= 0) {
			close(_listenFd);
			_listenFd = -1;
		}
	}

} // namespace Server
#endif
