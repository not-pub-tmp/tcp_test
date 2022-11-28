#include <mutex>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>

#include "linux_socket.h"
#include "echo.h"

using namespace std;

namespace TCP_TEST
{
using uniqueSock = std::unique_ptr<Socket>;

void ECHO::addSocket(uniqueSock&& newSock) {

  std::lock_guard<std::mutex> lock(mMutex);
  if(mSocketVec.size() < m_max_sockets) {
    mSocketVec.emplace_back(std::move(newSock));
  }
  else {
    throw(SocketException("Reach the limitation of maximum sockets"));
  }
}

void ECHO::setMaxSockets(uint32_t max_sockets) {
  m_max_sockets = max_sockets;
}

void ECHO::rmBadSocket(int32_t sock_fd) {
  std::lock_guard<std::mutex> lock(mMutex);
  auto it = remove_if(mSocketVec.begin(), mSocketVec.end(),
                [=](uniqueSock& sock){return sock.get()->getSocket() == sock_fd;});
  if(it != mSocketVec.end())
    mSocketVec.erase(it, mSocketVec.end());
}

void ECHO::echo() {
  std::array<uint8_t, 1500> recv_buf;
	memcpy(&recv_buf[0], "Echo from server", 17 - 1);

  bool empty;
  std::vector<std::reference_wrapper<const uniqueSock>> refSockets;
  refSockets.reserve(m_max_sockets);

  for(;;) {
    {
      // Determine if no valid sockets for echoing, if yes then sleep for a while
      {
        std::lock_guard<std::mutex> lock(ECHO::getInstance().mMutex);
        empty = ECHO::getInstance().mSocketVec.empty();
      }
      if(empty) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
      }

      // create local copy of the reference of echo sockets from the shared vector for futher processing
      {
        std::lock_guard<std::mutex> lock(ECHO::getInstance().mMutex);
        std::for_each(mSocketVec.begin(), mSocketVec.end(), 
                    [&](const uniqueSock &e) {
                  refSockets.push_back(std::cref<uniqueSock>(e));});
      }

      // use select to monitor all echo sockets to read data
      try {
        for(auto& it : mSocketVec) {
          auto len = it.get()->receiveData(recv_buf.data() + 16, recv_buf.size());
          if(0 < len) {
            std::cout << "Got sth = " << recv_buf.data() + 16 << std::endl;
          }
          else {
            throw(SocketException("Socket recv len = 0"));
          }
          it.get()->sendData(recv_buf.data(), len);
        } 
      } catch (const SocketException& e) {
        cout << "Exception is " << e.what() << endl;
        mSocketVec.erase(mSocketVec.begin(), mSocketVec.end());
      }
    }
  }
}
}