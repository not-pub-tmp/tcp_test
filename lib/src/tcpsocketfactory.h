#pragma once

#include "socketfactory.h"

namespace TCP_TEST
{

class TcpV4SocketFactory : public SocketFactory
{
public:
	static SocketFactory& getFactory() {
		static TcpV4SocketFactory factory;
		return factory;
	}

	virtual ~TcpV4SocketFactory() = default;
	virtual std::unique_ptr<Socket> createSock() override
	{
		return std::make_unique<Socket>(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	};

	virtual std::unique_ptr<Socket> createSock(int32_t sock) override
	{
		return std::make_unique<Socket>(sock);
	};
private:
	TcpV4SocketFactory() = default;

};
}
