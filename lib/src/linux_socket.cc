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

using namespace std;

namespace TCP_TEST
{
using AddrResPtr = unique_ptr<struct addrinfo, decltype(&freeaddrinfo)>;

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
  cout << " detor socket=" << socket_fd << endl;
  close(socket_fd);       //TODO Add more exception handler to deal with close error when packets in the queue
}

int32_t Socket::receiveData(void* buf, size_t len) {
  int32_t recv_len;

	recv_len = recv(socket_fd, buf, len, 0);
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

void Socket::connectTo(const string &host, const string &port)
{
	struct sockaddr_in serv_addr;
  int32_t port_no;

  try {
    port_no = stoi(port);
  }
  catch (invalid_argument& e) {
    throw(SocketException("Socket: invalid port number"));
  }

	memset(&serv_addr, 0, sizeof(serv_addr));
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

void Socket::bindSocket(const string &port)
{
	struct sockaddr_in serv_addr;
  int port_no;

  try {
    port_no = stoi(port);
  }
  catch (invalid_argument& e) {
    throw(SocketException("Socket: invalid port number"));
  }

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port_no);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  auto ret = bind(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (ret != 0) {
    int32_t err = errno;

    string errstr = string("Socket: bind error = ") + strerror(err);    
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

    string errstr = string("Socket: listen error = ") + strerror(err);
    throw(SocketException(errstr));
  }
}

unique_ptr<Socket> Socket::acceptConnection()
{
  socklen_t clientlen;
  struct sockaddr_in client_addr;
  int32_t newsockfd = accept(socket_fd, (struct sockaddr *)&client_addr, &clientlen);
  if(0 >= newsockfd) {
    int32_t err = errno;

    string errstr = string("Socket: accept error = ") + strerror(err);
    throw(SocketException(errstr));
  }
  unique_ptr<Socket> clientSock = TcpV4SocketFactory::getFactory().createSock(newsockfd);
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

