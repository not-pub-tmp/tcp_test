#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include "message_format.h"

using namespace std;

namespace TCP_TEST
{
int32_t PACKET::getpacketLen(uint8_t* buf, uint32_t len)
{
  array<uint8_t, 4> input{buf[0], buf[1], buf[2], buf[3]};

  // Only header was received, continue receiving
  if(len <= 8)
    return -1;

  // TODO Add more logic to resync the header
  if(input != prefix)
    return -1;

  uint32_t packetLen = 0;
  memcpy(&len, &buf[4], sizeof(uint32_t));

  packetLen = ntohl(packetLen);

  // The whole message was received 
  if(packetLen <= len)
    return packetLen;
}
}