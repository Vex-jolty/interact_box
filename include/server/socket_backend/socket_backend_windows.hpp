// net/socket_backend_win.hpp
#pragma once
#ifdef WIN32
	#include "socket_backend.hpp"
	#include <winsock2.h>
	#include <ws2tcpip.h>

namespace Server {

	class WinSocketBackend : public SocketBackend {
		public:
			bool init() override;
			bool bindAndListen(const std::string& host, int port) override;
			int acceptClient(std::string& clientIp) override;
			int recvData(int clientFd, std::string& out) override;
			void sendData(int clientFd, const std::string& data) override;
			void closeClient(int clientFd) override;
			void shutdown() override;

		private:
			SOCKET listenSocket = INVALID_SOCKET;
	};

} // namespace Server
#endif
