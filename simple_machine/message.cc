#include <unistd.h>
#include <iostream>
#include "message.h"

namespace vm {

void Message::PushMessage(void) const {
  ssize_t res = write(write_pipe_, &msg_, sizeof(msg_));
#if _DEBUG
  //std::clog << "write message res: " << res << std::endl;
#endif
}

void Message::PullMessage(void) {
  ssize_t res = read(read_pipe_, &msg_, sizeof(msg_));
#if _DEBUG
  //std::clog << "read message res: " << res << std::endl;
#endif
}

void Message::SetMessage(const MessageType &type,
                         const MessagePart &message_part) {
  Clear();
  msg_.type_ = type;
  msg_.message_ = message_part;
}

void Message::SetupWriteMessage(const int32_t& address,
                                const int32_t& data,
                                const CPUMode& cpu_mode,
                                MessagePart& message_part) {
  message_part.request_part_.command_type_ = WriteMemory;
  message_part.request_part_.running_mode_ = cpu_mode;
  message_part.request_part_.write_command_.memory_address_ = address;
  message_part.request_part_.write_command_.data_ = data;
}

} // namespace vm
