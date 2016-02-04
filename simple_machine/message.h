#ifndef SMPL_MCHN_MESSAGE_H_
#define SMPL_MCHN_MESSAGE_H_

#include <array>
#include <cstdint>

namespace vm {

typedef std::array<int32_t, 2000>::size_type MemoryAddress;

enum Command {
  Read = 1 << 0,
  Write = 1 << 1
};

enum MessageType {
  Request = 1 << 0,
  Respond = 1 << 1
};

struct RequestMessage {
  MemoryAddress location_;
  Command command_;
};

struct RespondMessage {
  int32_t data_;
};

struct MessageContent {
  MessageType type_;
  union {
    RequestMessage req_;
    RespondMessage rep_;
  };
};

class Message {
public:
  Message(const int& read_pipe, const int& write_pipe)
    : read_pipe_(read_pipe),
      write_pipe_(write_pipe) {
  }

  void PushMessage(MessageContent& message);
  void PullMessage(MessageContent& message);

private:
  const int read_pipe_;
  const int write_pipe_;
};

} // namespace vm

#endif // SMPL_MCHN_MESSAGE_H_
