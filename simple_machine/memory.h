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
  enum Status {
    MemoryIniting   = 1 << 0,
    MemoryRunning   = 1 << 1,
    MemoryExceptionPush = 1 << 2,
    MemoryExceptionPull = 1 << 3,
    MemoryEnding    = 1 << 4
  };
  Memory(const int& read_pipe,
         const int& write_pipe,
         const std::string& file_path)
    : message_(read_pipe, write_pipe),
      loader_pointer_(0),
      file_path_(file_path),
      status_(MemoryIniting) {
  }

  void Init(void);

  bool IsEnd(void) const {
    return (MemoryEnding == status_);
  }
  void PullRequest(void);
  void PrepareRespond(void);
  void PushRespond(void);


private:
  std::array<int32_t, 2000> memory_array_;
  int32_t loader_pointer_;
  std::ifstream program_file_;
  std::string file_path_;
  Status status_;

  // internal helper
  RetValue Init_(void);
  bool IsEnd_(void) const {
    return (message_.GetType() == Request &&
        message_.GetRequestCommandType() == EndProcess);
  }
  std::string StatusToString(void);

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
