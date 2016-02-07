#ifndef SMPL_MCHN_MESSAGE_H_
#define SMPL_MCHN_MESSAGE_H_

#include <array>
#include <cstdint>

namespace vm {

typedef std::array<int32_t, 2000>::size_type MemoryAddress;

/*** Request Message ***/

enum CommandType {
  InvalidCommand = 0,
  ReadMemory = 1 << 0,
  WriteMemory = 1 << 1,
  EndProcess = 1 << 2
};

struct ReadCommand {
  MemoryAddress address_offset_;
};

struct WriteCommand {
  MemoryAddress memory_address_;
  int32_t data_;
};

struct RequestMessage {
  CommandType command_type_;
  union {
    ReadCommand read_command_;
    WriteCommand write_command_;
  };
};

/*** Respond Message ***/
struct RespondMessage {
  int32_t data_;
};

/*** Top level Message ***/
enum MessageType {
  Invalid = 0,
  Request = 1 << 0,
  Respond = 1 << 1
};

struct MessageContent {
  MessageType type_;
  union {
    RequestMessage request_part_;
    RespondMessage respond_part_;
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
