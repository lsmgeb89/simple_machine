#include <unistd.h>
#include <iostream>
#include "message.h"

namespace vm {

void Message::PushMessage(void) const {
  ssize_t res = write(write_pipe_, &msg_, sizeof(msg_));
  if (-1 == res) {
    perror(LOG_ERROR_MODULE_MESSAGE"[write]");
  }
}

void Message::PullMessage(void) {
  ssize_t res = read(read_pipe_, &msg_, sizeof(msg_));
  if (-1 == res) {
    perror(LOG_ERROR_MODULE_MESSAGE"[read]");
  }
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

RetValue Message::GetRespondData(int32_t& data) const {
  if (GetType() != Respond) {
    error_message << "Wrong type of respond message!" << std::endl;
    return WrongMessageType;
  }

  if (IsSuccess(msg_.message_.respond_part_.OpResult)) {
    data = msg_.message_.respond_part_.data_;
  }
  return msg_.message_.respond_part_.OpResult;
}

} // namespace vm
