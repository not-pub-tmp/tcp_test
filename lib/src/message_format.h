#pragma once

#include <array>
#include <cstdint>

namespace TCP_TEST
{
const std::string echoServMessage{"Echo from server. "};
const uint32_t PREFIXLEN = 4;
constexpr uint32_t PACKETHEADERLEN = PREFIXLEN + sizeof(uint32_t);
constexpr uint32_t MAXBUFLEN = 1500 + PACKETHEADERLEN;
class PACKET
{
public:
  int32_t   writeBuf(const char*, uint32_t);
  uint32_t  readBuf(uint8_t*, uint32_t);
  uint8_t*  getRecvBuf();
  uint32_t  appendEchoMessage(uint32_t);
  uint8_t*  getBuf();
  const uint8_t*  getpayloadBuf() const;
  int32_t   getpacketLen(const uint8_t* const, const uint32_t);
private:
  const std::array<uint8_t, PREFIXLEN>  mprefix{0xda, 0xda, 0xda, 0xda};
  std::array<uint8_t, MAXBUFLEN>        mbuf;    //TODO Could use protobuf message 
};
}