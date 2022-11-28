#pragma once

#include <mutex>
#include <vector>
#include <memory>
#include <algorithm>
#include <fcntl.h>

#include "linux_socket.h"
#include "tcpsocketfactory.h"

namespace TCP_TEST
{
using uniqueSock = std::unique_ptr<Socket>;

class ECHO
{
public:
  static ECHO& getInstance() {
    static ECHO echo;
    return echo;
  }
  virtual ~ECHO() = default;
  void addSocket(std::unique_ptr<Socket>&&);  // Add socket accepted by main thread to the echo thread
  void rmDeadSocket(SOCKET);  // Erase the dead sockets from monitor list
  void setMax(uint32_t);    // Method to set the capacity of sockets handled by thread
  void echo();              // Thread function to echo the request from client connection

private:
  ECHO() = default;
  std::mutex     mMutex;            // Mutex to protect the access of shared vector of mSocketVec
  uint32_t       mMaxSockets;     // the capacity of sockets to be echoed simultaneously
  std::vector<uniqueSock>      mSocketVec; // accepted sockets by main thread
  const std::string  pipeName{"/tmp/fifo"};
};
}