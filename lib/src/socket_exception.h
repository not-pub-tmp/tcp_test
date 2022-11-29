#pragma once

#include <string>

namespace TCP_TEST
{
class SocketException : public std::exception {
private:
  std::string message;

public:
	SocketException(std::string msg)
								:message(msg) {};
  const char* what () const noexcept {
      return message.c_str();
  };
};
}