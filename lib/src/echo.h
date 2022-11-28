#pragma once

#include <mutex>
#include <vector>
#include <memory>
#include <algorithm>

#include "linux_socket.h"

namespace TCP_TEST
{
class ECHO
{
public:
  static ECHO& getInstance() {
    static ECHO echo;
    return echo;
  }
  virtual ~ECHO() = default;
  void addSocket(std::unique_ptr<Socket>&& newSock); 
  void rmBadSocket(int32_t sock_fd);
  void setMaxSockets(uint32_t max_sockets);
  void echo();

private:
  ECHO() = default;
  std::mutex     mMutex;
  uint32_t       m_max_sockets;
  std::vector<std::unique_ptr<Socket>>      mSocketVec;
};
}