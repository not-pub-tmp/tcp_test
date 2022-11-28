#pragma once

#include <memory>

#include <netinet/in.h>

#include "linux_socket.h"

namespace TCP_TEST
{

class SocketFactory 
{
public:
	SocketFactory() {};
	virtual ~SocketFactory() = default;
	virtual std::unique_ptr<Socket> createSock() = 0;
	virtual std::unique_ptr<Socket> createSock(int32_t) = 0;
};
}
