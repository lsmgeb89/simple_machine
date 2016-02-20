#ifndef SMPL_MCHN_MEMORY_H_
#define SMPL_MCHN_MEMORY_H_

#include <array>
#include <fstream>
#include <string>
#include "cpu.h"
#include "message.h"

namespace vm {

class Memory {
public:
  Memory(const int& read_pipe,
         const int& write_pipe,
         const std::string& file_path)
    : message_(read_pipe, write_pipe),
      file_path_(file_path) {
  }

  void Init(void);
  void PullRequest(void);
  void PrepareRespond(void);
  void PushRespond(void);
  bool IsEnd(void) const {
    return (message_.GetType() == Request &&
      message_.GetRequestCommandType() == EndProcess);
  }

private:
  std::array<int32_t, 2000> memory_array_;
  int32_t loader_pointer_;
  std::ifstream program_file_;
  std::string file_path_;

  // message related
  Message message_;

  RetValue Read(const int32_t& address_offset,
                const CPUMode& cpu_mode,
                int32_t& data);
  RetValue Write(const int32_t& address_offset,
                 const int32_t& val,
                 const CPUMode& cpu_mode);

  RetValue GrantPermission(const int32_t& address_offset,
                           const CPUMode& cpu_mode);

  bool IsInUserSpace (const int32_t& address) {
    return (address >= UserSpaceBegin &&
            address <= UserSpaceEnd);
  }

  bool IsInTimerSpace (const int32_t& address) {
    return (address >= TimerSpaceBegin &&
            address <= TimerSpaceEnd);
  }

  bool IsInSystemSpace (const int32_t& address) {
    return (address >= SystemSpaceBegin &&
            address <= SystemSpaceEnd);
  }
};

} // namespace vm

#endif // SMPL_MCHN_MEMORY_H_
