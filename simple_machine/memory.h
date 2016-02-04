#ifndef SMPL_MCHN_MEMORY_H_
#define SMPL_MCHN_MEMORY_H_

#include <array>
#include <fstream>
#include <string>
#include "message.h"

namespace vm {

typedef std::array<int32_t, 2000>::size_type MemoryAddress;

class Memory {
public:
  Memory(const int& read_pipe, const int& write_pipe)
    : message_(read_pipe, write_pipe) {
  }

  void Load(const std::string &file_path);

  void PullRequest(void);
  void PushRespond(void);
  void DoCommand(void);
  bool IsEnd(void);

private:
  std::array<int32_t, 2000> memory_array_;
  MemoryAddress loader_pointer_;
  std::ifstream program_file_;
  Message message_;

  void Read(void);
  void Write(void);
};

} // namespace vm

#endif // SMPL_MCHN_MEMORY_H_
