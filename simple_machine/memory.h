#ifndef SMPL_MCHN_MEMORY_H_
#define SMPL_MCHN_MEMORY_H_

#include <array>
#include <fstream>
#include <string>
#include "cpu.h"
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
  void PrepareRespond(void);
  void PushRespond(void);
  bool IsEnd(void) const {
    return (message_.GetType() == Request &&
      message_.GetRequestCommandType() == EndProcess);
  }

private:
  std::array<int32_t, 2000> memory_array_;
  MemoryAddress loader_pointer_;
  std::ifstream program_file_;

  // message related
  Message message_;

  RetValue Read(const MemoryAddress& address_offset,
                const CPUMode& cpu_mode,
                int32_t& data);
  RetValue Write(const MemoryAddress& address_offset,
                 const int32_t& val,
                 const CPUMode& cpu_mode);

  RetValue GrantPermission(const MemoryAddress& address_offset,
                           const CPUMode& cpu_mode);

  bool IsInUserSpace (const MemoryAddress& address) {
    return (address >= UserSpaceBegin &&
            address <= UserSpaceEnd);
  }

  bool IsInTimerSpace (const MemoryAddress& address) {
    return (address >= TimerSpaceBegin &&
            address <= TimerSpaceEnd);
  }

  bool IsInSystemSpace (const MemoryAddress& address) {
    return (address >= SystemSpaceBegin &&
            address <= SystemSpaceEnd);
  }
};

} // namespace vm

#endif // SMPL_MCHN_MEMORY_H_
