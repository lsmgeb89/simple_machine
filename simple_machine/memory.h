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
    : message_(read_pipe, write_pipe),
      need_push_(false) {
  }

  void Load(const std::string &file_path);

  void PullRequest(void);
  void PushRespond(void);
  bool IsEnd(void) const {
    return (message_.GetType() == Request &&
      message_.GetRequestCommandType() == EndProcess);
  }

  bool NeedPush(void) const { return need_push_; }

private:
  std::array<int32_t, 2000> memory_array_;
  MemoryAddress loader_pointer_;
  std::ifstream program_file_;

  // message related
  Message message_;
  bool need_push_;

  void PrepareRespond(void);
  int32_t Read(const MemoryAddress& address_offset);
  void Write(const MemoryAddress& address_offset, const int32_t& val);
};

} // namespace vm

#endif // SMPL_MCHN_MEMORY_H_
