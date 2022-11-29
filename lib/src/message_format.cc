#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <iomanip>

#include "message_format.h"
#include "socket_exception.h"

using namespace std;

namespace TCP_TEST
{
int32_t PACKET::getpacketLen(const uint8_t* const buf, const uint32_t len)
{
  array<uint8_t, 4> input{buf[0], buf[1], buf[2], buf[3]};

  // Only header was received, continue receiving
  if(len <= 8)
    throw(SocketException("Socket: only part was received"));

  // TODO Add more logic to resync the header
  if(input != mprefix) 
    throw(SocketException("Socket: no correct prefix"));

  uint32_t packetLen = 0;
  memcpy(&packetLen, &buf[4], sizeof(uint32_t));

  packetLen = ntohl(packetLen);

  // Only part of message was received 
  if(packetLen > len)
    throw(SocketException("Socket: only part was received"));

  return packetLen;
}

int32_t PACKET::writeBuf(const char* buf, uint32_t len)
{
  if(nullptr == buf)
    throw(SocketException("Socket: nullptr"));

  uint32_t totalLen = len + PACKETHEADERLEN;

  if(totalLen > mbuf.size()) 
    throw(SocketException("Socket: input buffer is too long"));
  
  memcpy(mbuf.data(), mprefix.data(), mprefix.size());
  
  uint32_t networkLen = htonl(len + mprefix.size() + sizeof(uint32_t));
  memcpy(mbuf.data() + mprefix.size(), reinterpret_cast<uint8_t*>(&networkLen), sizeof(networkLen));

  memcpy(mbuf.data() + PACKETHEADERLEN, buf, len);

  return totalLen;
}

uint8_t* PACKET::getRecvBuf()
{
  return mbuf.data() + echoServMessage.size(); // Reserve space for echo message to decrease memcpy
}

uint32_t PACKET::appendEchoMessage(uint32_t packetLen)
{
  memcpy(mbuf.data(), mprefix.data(), mprefix.size());

  uint32_t networkLen = htonl(packetLen + echoServMessage.size());

  memcpy(mbuf.data() + mprefix.size(), reinterpret_cast<uint8_t*>(&networkLen), sizeof(networkLen));

  memcpy(mbuf.data() + PACKETHEADERLEN, echoServMessage.c_str(), echoServMessage.size());

  return packetLen + echoServMessage.size();
}

uint8_t* PACKET::getBuf()
{
  return mbuf.data();
}

const uint8_t* PACKET::getpayloadBuf() const
{
  return mbuf.data() + PACKETHEADERLEN;
}
}