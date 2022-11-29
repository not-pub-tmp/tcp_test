#include <mutex>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "linux_socket.h"
#include "echo.h"
#include "message_format.h"

using namespace std;

namespace TCP_TEST
{

void ECHO::addSocket(uniqueSock&& newSock) {

  lock_guard<mutex> lock(mMutex);
  if(mSocketVec.size() < mMaxSockets) {
    mSocketVec.emplace_back(move(newSock));
  }
  else {
    throw(SocketException("Reach the limitation of maximum sockets"));
  }

// TODO release pipe handle graefully
  static int32_t fd = -1;
  if(fd == -1)
  fd = open(pipeName.c_str(), O_WRONLY);
  write(fd, "Hi", sizeof("Hi"));
}

void ECHO::setMax(uint32_t max) {
  mMaxSockets = max;
}

void ECHO::rmDeadSocket(SOCKET sockFd) {

  lock_guard<mutex> lock(ECHO::getInstance().mMutex);

  // remove socket and release the socket which len=0 returned by recv which means the client socket was closed
  auto it_rm = remove_if(mSocketVec.begin(), mSocketVec.end(),
                        [&sockFd](auto& p){return p->getSocket() == sockFd;});
  if(it_rm != mSocketVec.end())
    mSocketVec.erase(it_rm, mSocketVec.end());
}

void ECHO::echo() {

  SOCKET processingSock = 0;

  /* create the FIFO (named pipe) */
  mkfifo(pipeName.c_str(), 0666);
  auto pipeFd = open(pipeName.c_str(), O_RDONLY);

  vector<reference_wrapper<const uniqueSock>> refSockets;
  refSockets.reserve(mMaxSockets);

  PACKET recvPacket;

  for(;;) {

    SOCKET maxFd;
    fd_set readFds;
    FD_ZERO(&readFds);

    auto max_Socket = max_element(refSockets.cbegin(), refSockets.cend(),
                          [](auto a, auto b){ return a.get()->getSocket() < b.get()->getSocket(); });
    if(max_Socket != refSockets.cend())
      maxFd = max_Socket->get()->getSocket();

    for_each(refSockets.cbegin(), refSockets.cend(),
              [&readFds](auto it){ FD_SET(it.get()->getSocket(), &readFds);});


    FD_SET(pipeFd, &readFds);
    maxFd = max(maxFd, pipeFd);

    // use select to monitor all echo sockets and the pipe file to read data and update socket list
    try {
      int32_t activity = select(maxFd + 1, &readFds, NULL, NULL, NULL);

      switch (activity) {
        case -1:
        case 0:
          throw(SocketException("Socket: Select error"));

        default:
          array<uint8_t, 10> pipeRecvBuf;
          // main thread modified the shared vector
          if (FD_ISSET(pipeFd, &readFds)) {
            read(pipeFd, pipeRecvBuf.data(), pipeRecvBuf.size());
            // create local copy of the reference of echo sockets from the shared vector for futher processing
            lock_guard<mutex> lock(ECHO::getInstance().mMutex);

            refSockets.clear();
            for_each(mSocketVec.begin(), mSocketVec.end(),
                          [&](const uniqueSock &e) {
                          refSockets.push_back(cref<uniqueSock>(e));});
          }

          for(auto it = refSockets.cbegin(); it != refSockets.cend(); ++it) {
            processingSock = it->get()->getSocket();

            if(FD_ISSET(processingSock, &readFds)) {
              auto recvLen = it->get()->receiveData(recvPacket.getRecvBuf(), MAXBUFLEN - echoServMessage.size());
              if(0 >= recvLen)
                throw(SocketException("Socket recv len = 0"));

              auto sendLen = recvPacket.appendEchoMessage(recvLen);

              it->get()->sendData((void*)recvPacket.getBuf(), sendLen);
            }
          }
        }

      FD_ZERO(&readFds);

    } catch (const SocketException& e) {
      cout << "Exception is " << e.what() << endl;
      rmDeadSocket(processingSock);

      // update internal vector of reference to synconize the shared vector
      refSockets.clear();
      lock_guard<mutex> lock(ECHO::getInstance().mMutex);
      for_each(mSocketVec.begin(), mSocketVec.end(),
                    [&](const uniqueSock &p) {
                    refSockets.push_back(cref<uniqueSock>(p));});

    } catch (const exception& e) {
      cout << "Exception is " << e.what() << endl;
    }
  }
}
}