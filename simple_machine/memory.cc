#include <iostream>
#include <sstream>
#include "memory.h"

namespace vm {

void Memory::Load(const std::string &file_path) {
  std::string line;

  // put into constructor
  loader_pointer_ = 0;
  uint32_t line_num = 1;

  // open a program file
  program_file_.open(file_path);

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
  message_.PullMessage(msg_);
  if (!IsEnd()) {
    PrepareRespond();
  } else {
    std::cout << "Got End!" << std::endl;
  }
}

void Memory::PushRespond(void) {
  if (!IsEnd()) {
    message_.PushMessage(msg_);
  }
}

int32_t Memory::Read(const MemoryAddress& address_offset) {
  return memory_array_[address_offset];
}

void Memory::Write(const MemoryAddress& address_offset, const int32_t& val) {
  memory_array_[address_offset] = val;
}

void Memory::PrepareRespond(void) {
  switch (msg_.type_) {
    case Request:
      if (msg_.request_part_.command_type_ == ReadMemory) {
        // prepare a respond message for read request
        msg_.type_ = Respond;
        msg_.respond_part_.data_ = Read(msg_.request_part_.read_command_.address_offset_);
      } else if (msg_.request_part_.command_type_ == WriteMemory) {
        // there is no need to prepare a respond message for write request
        Write(msg_.request_part_.write_command_.memory_address_,
              msg_.request_part_.write_command_.data_);
      }
      break;
    case Respond:
      std::cerr << "[error] Memory should not receive a respond message!" << std::endl;
      break;
    default:
      break;
  }
}

} // namespace vm
