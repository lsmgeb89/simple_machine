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
      register_sp_(USER_STACK),
      register_ir_(0),
      register_ac_(0),
      register_x_(0),
      register_y_(0),
      message_(read_pipe, write_pipe) {
  }

  void FetchNextInstruction(void);
  void ExecuteInstruction(void);
  bool IsEnd(void) const { return (register_ir_ == 50); }

private:
  // registers
  int32_t register_pc_;
  int32_t register_sp_;
  int32_t register_ir_;
  int32_t register_ac_;
  int32_t register_x_;
  int32_t register_y_;

  // message related
  MessageContent msg_;
  Message message_;

  // Internal communication function
  void PushRequest(const CommandType& command_type,
                   const int32_t& memory_address);
  void PullRespond(int32_t& data);

  /*** instructions ***/
  void LoadValue(void);
  void CopyToX(void);
  void CopyToY(void);
  void LoadIdxX(void);
  void LoadIdxY(void);
  void AddY(void);
  void JumpIfEqual(void);
  void Put(void);
  void IncX(void);
  void Jump(void);
  void End(void);

  void LoadIdx(const int32_t& register_);

  // debug helper
  std::string RegisterToString(void);
};

} // namespace vm

#endif // SMPL_MCHN_CPU_H_
