#include <iostream>
#include <sstream>
#include "memory.h"

namespace vm {

void Memory::Init(void) {
  std::string line;

  // put into constructor
  loader_pointer_ = 0;
#if _DEBUG
  uint32_t line_num = 1;
#endif

  // open a program file
  program_file_.open(file_path_);

  // TODO: more error handling
  if (program_file_) {
    while (std::getline(program_file_, line)) {
      bool change_loader = false;
#if _DEBUG
      std::clog << "[" << line_num++ << "]" << line << std::endl;
#endif

      // skip empty lines
      if (line.empty()) {
        continue;
      }

      // non-empty indicate at least one to check whether it need change loader address
      if (*line.cbegin() == '.') {
        change_loader = true;
        line.erase(0, 1);
      }

      bool find_digit = false;

      auto it = line.cbegin();
      for (; it != line.cend(); ++it) {
        if (!isdigit(*it)) {
          if (find_digit) {
            // find the first non-digit character in this line
            break;
          } else if (isspace(*it)) {
            continue;
          } else if ((*it == '/') && (*(it + 1) == '/')) {
#ifdef _DEBUG
            std::clog << "[info] find valid comment!" << std::endl;
#endif
            break;
          } else {
            std::cerr << "[error] Invalid input program file!" << std::endl;
            // TODO: add proper error handling
            return;
          }
        } else if (!find_digit) {
          // find digit characters and maintain the status indicator
          find_digit = true;
        }
      }

      if (find_digit) {
        // discard the remaining string content
        line.erase(it - line.cbegin(), line.cend() - it);
        if (change_loader) {
#if _DEBUG
          std::clog << "Change loader address to " << line << std::endl;
#endif
          loader_pointer_ = std::stoul(line);
        } else {
#if _DEBUG
          std::clog << "memory[" << loader_pointer_ << "] = " << line << std::endl;
#endif
          memory_array_[loader_pointer_++] = std::stoi(line);
        }
      }
    }
  }

  // close
  program_file_.close();
}

void Memory::PullRequest(void) {
  message_.PullMessage();
}

void Memory::PushRespond(void) {
  if (!IsEnd()) {
    message_.PushMessage();
  }
}

RetValue Memory::GrantPermission(const MemoryAddress& address,
                                 const CPUMode& cpu_mode) {

  if ((cpu_mode == UserMode && IsInUserSpace(address)) ||
      (cpu_mode == TimerMode && IsInTimerSpace(address)) ||
      (cpu_mode == SystemMode && IsInSystemSpace(address))) {
    return Success;
  } else {
    return MemoryViolation;
  }
}

RetValue Memory::Read(const MemoryAddress& address,
                      const CPUMode& cpu_mode,
                      int32_t& data) {
  RetValue ret = GrantPermission(address, cpu_mode);
  if (IsSuccess(ret)) {
    data = memory_array_[address];
#if _DEBUG
    std::clog << "[info] Read " << data << " @ " << address << std::endl;
#endif
  } else {
    std::cerr << "[error] Read Memory Violation: " << address << std::endl;
  }
  return ret;
}

RetValue Memory::Write(const MemoryAddress& address,
                       const int32_t& val,
                       const CPUMode& cpu_mode) {
  RetValue ret = GrantPermission(address, cpu_mode);
  if (IsSuccess(ret)) {
    memory_array_[address] = val;
#if _DEBUG
    std::clog << "[info] Write " << val << " @ " << address << std::endl;
#endif
  } else {
    std::cerr << "[error] Write Memory Violation @ " << address << ", mode: " << cpu_mode << std::endl;
  }
  return ret;
}

void Memory::PrepareRespond(void) {
  MessagePart message_part;
  MessageType type = message_.GetType();

  if (Request == type) {
    if (message_.GetRequestCommandType() == ReadMemory) {
      // prepare a respond message for read request
      message_part.respond_part_.OpResult =
        Read(message_.GetReadRequest(),
             message_.GetRequestCommandMode(),
             message_part.respond_part_.data_);
    } else if (message_.GetRequestCommandType() == WriteMemory) {
      // prepare a respond message for write request
      message_part.respond_part_.OpResult =
        Write(message_.GetWriteRequestAddress(),
              message_.GetWriteRequestData(),
              message_.GetRequestCommandMode());
    } else {
      std::cout << "[Info] Got End!" << std::endl;
      return;
    }
  } else {
    std::cerr << "[error] Memory should not receive a respond message!" << std::endl;
    message_part.respond_part_.OpResult = WrongMessageType;
  }
  message_.SetMessage(Respond, message_part);
}

} // namespace vm
