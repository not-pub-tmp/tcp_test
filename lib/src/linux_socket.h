#pragma once

#include <cstdint>
#include <memory>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "socket_exception.h"

namespace TCP_TEST
{
using SOCKET  = int32_t;

class Socket
{
public:
	Socket(int32_t domain,         	// address family (for example, AF_INET)
         int32_t type,           	// SOCK_STREAM, SOCK_DGRAM, or SOCK_RAW
         int32_t protocol); 				// socket protocol

	Socket(int32_t sock);			 		//overload ctor to have a socket created outside

	~Socket();

	int32_t receiveData(void *buf, size_t len);
	int32_t sendData(const void *buf, size_t len);
	void connectTo(const std::string &host, const std::string &port);
	void disconnect();
	void bindSocket(const std::string &port);
	void listenConnection(const int32_t backlog);
	std::unique_ptr<Socket> acceptConnection();
	int32_t setNonBlockingIO(bool status);
	std::string getPort();
	sockaddr_in getPeerAddr();
	void setPeerAddr(sockaddr_in& peerAddr);
	SOCKET getSocket();

protected:
	SOCKET socket_fd = -1;
	sockaddr_in clientAddr;
};

}