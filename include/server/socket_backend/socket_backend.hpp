// net/socket_backend.hpp
#pragma once
#include <string>

namespace Server {

	class SocketBackend {
		public:
			virtual ~SocketBackend() = default;

			virtual bool init() = 0;
			virtual bool bindAndListen(const std::string& host, int port) = 0;
			virtual int acceptClient(std::string& clientIp) = 0;
			virtual int recvData(int clientFd, std::string& out) = 0;
			virtual void sendData(int clientFd, const std::string& data) = 0;
			virtual void closeClient(int clientFd) = 0;
			virtual void shutdown() = 0;
	};

} // namespace Server
