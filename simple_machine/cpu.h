#ifndef SMPL_MCHN_CPU_H_
#define SMPL_MCHN_CPU_H_

#include "message.h"

#define USER_STACK (1000)
#define SYSTEM_STACK (2000)

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
  Message message_;

  // Internal communication function
  void PushRequest(const MessagePart& message_part);
  void PullRespond(int32_t& data);

  /*** instructions ***/
  void LoadValue(void); // 1
  void LoadIdxX(void); // 4
  void LoadIdxY(void); // 5
  void LoadSpX(void); // 6
  void Put(void); // 9
  void AddY(void); // 10
  void CopyToX(void); // 14
  void CopyFromX(void); // 15
  void CopyToY(void); // 16
  void CopyFromY(void); // 17
  void Jump(void); // 20
  void JumpIfEqual(void); // 21
  void JumpIfNotEqual(void); // 22
  void CallAddr(void); // 23
  void Ret(void); // 24
  void IncX(void); // 25
  void DecX(void); // 26
  void Push(void); // 27
  void Pop(void); // 28
  void End(void); // 50

  // Internal helper
  void LoadIdx(const int32_t& register_);
  void MovePC(const int32_t& offset = 1) {
    register_pc_ += offset;
  }

  // debug helper
  std::string RegisterToString(void);
};

} // namespace vm

#endif // SMPL_MCHN_CPU_H_
