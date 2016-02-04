#ifndef SMPL_MCHN_CPU_H_
#define SMPL_MCHN_CPU_H_

#include "message.h"

#define USER_STACK (999)
#define SYSTEM_STACK (1999)

namespace vm {

class CPU {
public:
  CPU(const int& read_pipe, const int& write_pipe)
    : register_pc_(0),
      register_sp_(USER_STACK), // TODO: properly initialization sp
      register_ir_(0),
      register_ac_(0),
      register_x_(0),
      register_y_(0),
      message_(read_pipe, write_pipe) {
  }

  void PushRequest(void);
  void PullRespond(void);
  void DoCommand(void);
  bool IsEnd();

private:
  MemoryAddress register_pc_; //
  int32_t register_sp_;
  int32_t register_ir_;
  int32_t register_ac_;
  int32_t register_x_;
  int32_t register_y_;

  Message message_;
};

} // namespace vm

#endif // SMPL_MCHN_CPU_H_
