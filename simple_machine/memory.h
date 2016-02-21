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
    : file_path_(file_path),
      loader_pointer_(0),
      message_(read_pipe, write_pipe),
      status_(MemoryIniting) {
  }

  void Init(void);
  void PullRequest(void);
  void PrepareRespond(void);
  void PushRespond(void);
  bool IsEnd(void) const {
    return (MemoryEnding == status_);
  }

private:
  /*** Hardware stuffs ***/
  std::array<int32_t, 2000> memory_array_;

  // loader
  std::string file_path_;
  std::ifstream program_file_;
  int32_t loader_pointer_;

  // message for request and response
  Message message_;

  Status status_;

  // internal operations
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

  RetValue GrantPermission(const int32_t& address_offset,
                           const CPUMode& cpu_mode);

  RetValue Read(/* in  */ const int32_t& address_offset,
                /* in  */ const CPUMode& cpu_mode,
                /* out */ int32_t& data);

  RetValue Write(const int32_t& address_offset,
                 const int32_t& val,
                 const CPUMode& cpu_mode);
  
  // internal helpers
  RetValue Init_(void);
  bool IsEnd_(void) const {
    return (message_.GetType() == Request &&
        message_.GetRequestCommandType() == EndProcess);
  }

  // debug helpers
  std::string StatusToString(void) const;
};

} // namespace vm

#endif // SMPL_MCHN_MEMORY_H_
