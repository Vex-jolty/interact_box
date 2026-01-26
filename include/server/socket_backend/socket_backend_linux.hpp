// net/socket_backend_linux.hpp
#pragma once
#ifdef __linux__
	#include "socket_backend.hpp"
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>

namespace Server {

	class LinuxSocketBackend : public SocketBackend {
		public:
			bool init() override { return true; }

			bool bindAndListen(const std::string& host, int port) override;
			int acceptClient(std::string& clientIp) override;
			int recvData(int clientFd, std::string& out) override;
			void sendData(int clientFd, const std::string& data) override;
			void closeClient(int clientFd) override;
			void shutdown() override;

		private:
			int listenFd = -1;
	};

} // namespace Server
#endif
