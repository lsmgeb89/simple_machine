#ifndef SIMPLE_MACHINE_MESSAGE_H_
#define SIMPLE_MACHINE_MESSAGE_H_

#include <array>
#include <cstdint>
#include <cstring>
#include "common.h"
#include "util.h"

namespace vm {

/*** Request Message ***/
enum CommandType {
  ReadMemory = 1 << 0,
  WriteMemory = 1 << 1,
  EndProcess = 1 << 2
};

struct ReadCommand {
  int32_t address_offset_;
};

struct WriteCommand {
  int32_t memory_address_;
  int32_t data_;
};

struct RequestMessage {
  CommandType command_type_;
  CPUMode running_mode_;
  union {
    ReadCommand read_command_;
    WriteCommand write_command_;
  };
};

/*** Respond Message ***/
struct RespondMessage {
  RetValue OpResult;
  int32_t data_;
};

/*** Top level Message ***/
enum MessageType {
  Invalid = 0,
  Request = 1 << 0,
  Respond = 1 << 1
};

union MessagePart {
  RequestMessage request_part_;
  RespondMessage respond_part_;
};

struct MessageContent {
  MessageType type_;
  MessagePart message_;
};

class Message {
public:
  Message(const int& read_pipe, const int& write_pipe)
    : read_pipe_(read_pipe),
      write_pipe_(write_pipe) {
    Clear();
  }

  void PushMessage(void) const;
  void PullMessage(void);

  // Set
  void SetMessage(const MessageType& type,
                  const MessagePart& message_part);

  static void SetupWriteMessage(const int32_t& address,
                                const int32_t& data,
                                const CPUMode& cpu_mode,
                                MessagePart& message_part);

  // Get
  const MessageType& GetType(void) const {
    return msg_.type_;
  }

  const CommandType& GetRequestCommandType(void) const {
    return msg_.message_.request_part_.command_type_;
  }

  const CPUMode& GetRequestCommandMode(void) const {
    return msg_.message_.request_part_.running_mode_;
  }

  const int32_t& GetReadRequest(void) const {
      return msg_.message_.request_part_.read_command_.address_offset_;
  }

  const int32_t& GetWriteRequestAddress(void) const {
    return msg_.message_.request_part_.write_command_.memory_address_;
  }

  const int32_t& GetWriteRequestData(void) const {
    return msg_.message_.request_part_.write_command_.data_;
  }

  RetValue GetRespondData(int32_t& data) const;

  void Clear(void) {
    msg_.type_ = Invalid;
    std::memset(&msg_.message_, 0, sizeof(msg_.message_));
  }

private:
  const int read_pipe_;
  const int write_pipe_;
  MessageContent msg_;
};

} // namespace vm

#endif // SMPL_MCHN_MESSAGE_H_
