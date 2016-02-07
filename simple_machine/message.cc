#include <unistd.h>
#include <iostream>
#include "message.h"

namespace vm {

void Message::PushMessage(MessageContent& message) {
  ssize_t res = write(write_pipe_, reinterpret_cast<void *>(&message), sizeof(message));
  //std::cout << "write message res: " << res << std::endl;
  return;
}

void Message::PullMessage(MessageContent& message) {
  ssize_t res = read(read_pipe_, reinterpret_cast<void *>(&message), sizeof(message));
  //std::cout << "read message res: " << res << std::endl;
  return;
}

} // namespace vm
