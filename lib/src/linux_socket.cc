#include <stdexcept>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "linux_socket.h"
#include "tcpsocketfactory.h"

namespace TCP_TEST
{
using AddrResPtr = std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)>;

Socket::Socket(int32_t domain,         	// address family (for example, AF_INET)
        int32_t type,           	// SOCK_STREAM, SOCK_DGRAM, or SOCK_RAW
        int32_t protocol) 				// socket protocol
        {
        socket_fd = socket(domain, type, protocol);
        }

Socket::Socket(int32_t sock) {
  socket_fd = sock;
}

Socket::~Socket() {
  std::cout << " descontruct socket=" << socket_fd << std::endl;
  auto result = close(socket_fd);
  std::cout << "close socket=" << result << std::endl;
}

int32_t Socket::receiveData(void* buf, size_t len) {
  int32_t recv_len;

	recv_len = recv(socket_fd, buf, len, 0);
  if(recv_len < 0)
    throw(SocketException("Socket recv error"));
  else
    return recv_len;
}

int32_t Socket::receiveDat(uint32_t len) {
  int32_t recv_len;
  std::array<uint8_t, 1500> recv_buf;

	recv_len = recv(socket_fd, recv_buf.data(), recv_buf.size(), 0);
  if(recv_len < 0)
    throw(SocketException("Socket recv error"));
  else
    return recv_len;
}

int32_t Socket::sendData(const void *buf, size_t len) {
  int32_t send_len;

	send_len = send(socket_fd, buf, len, 0);
  if(send_len < 0)
    throw(SocketException("Socket send error"));
  else
    return send_len;
}

void Socket::connectTo(const std::string &host, const std::string &port)
{
	struct sockaddr_in serv_addr;
  int port_no;

  try {
    port_no = stoi(port);
  }
  catch (std::invalid_argument& e) {
    throw(SocketException("Socket: invalid port number"));
  }

	std::memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port_no);

  struct in_addr addr;
  auto result = inet_pton(AF_INET, host.c_str(), &addr);
  if(1 != result) {
    throw(SocketException("Socket: server ip error"));
  }

  serv_addr.sin_addr.s_addr= addr.s_addr;

  auto ret = connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (ret != 0)
		throw(SocketException("Socket: connect to sserver error"));
}

void Socket::bindSocket(const std::string &port)
{
	struct sockaddr_in serv_addr;
  int port_no;

  try {
    port_no = stoi(port);
  }
  catch (std::invalid_argument& e) {
    throw(SocketException("Socket: invalid port number"));
  }

	std::memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port_no);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  auto ret = bind(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (ret != 0) {
    int32_t err = errno;

    std::string errstr = std::string("Socket: bind error = ") + std::strerror(err);    
		throw(SocketException(errstr));
  }
}

void Socket::listenConnection(const int32_t backlog)
{
  if(0 >= backlog)
    throw(SocketException("Socket: listenConnection para error"));

  auto res = listen(socket_fd, backlog);
  if(0 != res) {
    int32_t err = errno;

    std::string errstr = std::string("Socket: listen error = ") + std::strerror(err);
    throw(SocketException(errstr));
  }
}

std::unique_ptr<Socket> Socket::acceptConnection()
{
  socklen_t clientlen;
  struct sockaddr_in client_addr;
  int32_t newsockfd = accept(socket_fd, (struct sockaddr *)&client_addr, &clientlen);
  if(0 >= newsockfd) {
    int32_t err = errno;

    std::string errstr = std::string("Socket: accept error = ") + std::strerror(err);
    throw(SocketException(errstr));
  }
  std::unique_ptr<Socket> clientSock = TcpV4SocketFactory::getFactory().createSock(newsockfd);
  clientSock->setPeerAddr(client_addr);
  return clientSock;
}

void Socket::setPeerAddr(sockaddr_in& peerAddr)
{
  this->clientAddr = peerAddr;
}

sockaddr_in Socket::getPeerAddr()
{
  return this->clientAddr;
}

SOCKET Socket::getSocket()
{
  return this->socket_fd;
}

}

