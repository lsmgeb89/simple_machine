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
      std::clog << "[" << line_num++ << "]" << line << std::endl;

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
          std::clog << "Change loader address to " << line << std::endl;
          loader_pointer_ = std::stoi(line);
        } else {
          std::clog << "memory[" << loader_pointer_ << "] = " << line << std::endl;
          memory_array_[loader_pointer_++] = std::stoi(line);
        }
      }
    }
  }

  // close
  program_file_.close();
}

void Memory::PullRequest(void) {
  MessageContent msg;
  message_.PullMessage(msg);
}

void Memory::Read(void) {

}

void Memory::Write(void) {

}

void Memory::PushRespond(void) {

}

void Memory::DoCommand(void) {

}

bool Memory::IsEnd(void) {
  return false;
}

} // namespace vm
