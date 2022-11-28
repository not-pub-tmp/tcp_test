#pragma once

#include <array>
#include <cstdint>

namespace TCP_TEST
{
const std::string echoServMessage{"Echo by Server:"};
const std::string clientMessage{"Hello server request from %d, expecting response"};
class PACKET
{
public:
  int32_t  getpacketLen(uint8_t* buf, uint32_t len);
  uint8_t*  getPayload();
private:
  const std::array<uint8_t, 4>  prefix{0xda, 0xda, 0xda, 0xda};
  std::string                   message;    //TODO Could use protobuf message 
};
}