#include <iostream>
#include <sstream>
#include "memory.h"

namespace vm {

void Memory::Init(void) {
  // Switch status
  if (IsSuccess(Init_())) {
    info_memory << "[status]" << StatusToString() << " ---> MemoryRunning" << std::endl;
    status_ = MemoryRunning;
  } else {
    info_memory << "[status]" << StatusToString() << " ---> MemoryExceptionPush" << std::endl;
    status_ = MemoryExceptionPush;
  }
}

RetValue Memory::Init_(void) {
  RetValue ret = Success;
  std::string line;

  // open a program file
  program_file_.open(file_path_);

  // parse it line by line
  if (program_file_) {
    while (std::getline(program_file_, line)) {
      bool change_loader = false;

      // skip an empty line
      if (line.empty()) {
        continue;
      }

      // Non-empty indicates there is at least one character
      // Check whether it need change loader address
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
            info_memory << "Find valid comment." << std::endl;
            break;
          } else {
            error_memory << "Invalid input program file." << std::endl;
            ret = InvalidProgramFile;
            goto done;
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
          info_memory << "Change loader address to " << line << std::endl;
          loader_pointer_ = std::stoi(line);
        } else {
          info_memory << "memory_array[" << loader_pointer_ << "] = " << line << std::endl;
          memory_array_[loader_pointer_++] = std::stoi(line);
        }
      }
    }
  } else {
    error_memory << "The file doesn't exist @ " << file_path_ << std::endl;
    ret = FileNotExisted;
  }

done:
  // close
  program_file_.close();
  return ret;
}

void Memory::PullRequest(void) {
  message_.PullMessage();
}

void Memory::PushRespond(void) {
  if (MemoryExceptionPull == status_ ||
      MemoryRunning == status_) {
    message_.PushMessage();
  }
}

RetValue Memory::GrantPermission(const int32_t& address,
                                 const CPUMode& cpu_mode) {

  if ((cpu_mode == UserMode && IsInUserSpace(address)) ||
      (cpu_mode == TimerMode && IsInTimerSpace(address)) ||
      (cpu_mode == SystemMode && IsInSystemSpace(address))) {
    return Success;
  } else {
    return MemoryViolation;
  }
}

RetValue Memory::Read(const int32_t& address,
                      const CPUMode& cpu_mode,
                      int32_t& data) {
  RetValue ret = GrantPermission(address, cpu_mode);
  if (IsSuccess(ret)) {
    data = memory_array_[address];
    info_memory << "Read " << data << " @ " << address << std::endl;
  } else {
    error_memory << "Read Memory Violation: accessing address " << address;
    std::cerr << " in " << ((cpu_mode == UserMode) ? "user mode" : "system mode") << std::endl;
  }
  return ret;
}

RetValue Memory::Write(const int32_t& address,
                       const int32_t& val,
                       const CPUMode& cpu_mode) {
  RetValue ret = GrantPermission(address, cpu_mode);
  if (IsSuccess(ret)) {
    memory_array_[address] = val;
    info_memory << "Write " << val << " @ " << address << std::endl;
  } else {
    error_memory << "Write Memory Violation: accessing address " << address;
    std::cerr << " in " << ((cpu_mode == UserMode) ? "user mode" : "system mode") << std::endl;
  }
  return ret;
}

void Memory::PrepareRespond(void) {
  MessagePart message_part;
  MessageType type = message_.GetType();

  if (MemoryExceptionPush == status_) {
    // Notify CPU that Memory will not work any more
    message_part.respond_part_.OpResult = MemoryError;
    info_memory << "[status]" << StatusToString() << " ---> MemoryExceptionPull" << std::endl;
    status_ = MemoryExceptionPull;
  } else if (MemoryExceptionPull == status_ && IsEnd_()) {
    info_memory << "Got end, exit for exception!" << std::endl;
    // Change status to ending
    info_memory << "[status]" << StatusToString() << " ---> MemoryEnding" << std::endl;
    status_ = MemoryEnding;
    return;
  } else if (MemoryRunning == status_) {
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
        info_memory << "Got end, normal exit!" << std::endl;
        // Change status to ending
        info_memory << "[status]" << StatusToString() << " ---> MemoryEnding" << std::endl;
        status_ = MemoryEnding;
        return;
      }

      // Change state to MemoryExceptionPull
      if (!IsSuccess(message_part.respond_part_.OpResult)) {
        info_memory << "[status]" << StatusToString() << " ---> MemoryExceptionPull" << std::endl;
        status_ = MemoryExceptionPull;
      }
    } else {
      error_memory << "Memory should not receive a respond message!" << std::endl;
      message_part.respond_part_.OpResult = WrongMessageType;
    }
  }

  message_.SetMessage(Respond, message_part);
}

std::string Memory::StatusToString(void) {
  std::string res;
  if (MemoryIniting == status_) {
    res = "MemoryIniting";
  } else if (MemoryRunning == status_) {
    res = "MemoryRunning";
  } else if (MemoryExceptionPush == status_) {
    res = "MemoryExceptionPush";
  } else if (MemoryExceptionPull == status_) {
    res = "MemoryExceptionPull";
  } else if (MemoryEnding == status_) {
    res = "MemoryEnding";
  }
  return res;
}

} // namespace vm
