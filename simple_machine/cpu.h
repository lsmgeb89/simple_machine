#ifndef SMPL_MCHN_CPU_H_
#define SMPL_MCHN_CPU_H_

#include "message.h"

namespace vm {

constexpr int32_t UserSpaceBegin(0);
constexpr int32_t UserSpaceEnd(999);
constexpr int32_t TimerSpaceBegin(1000);
constexpr int32_t TimerSpaceEnd(1999);
constexpr int32_t SystemSpaceBegin(1500);
constexpr int32_t SystemSpaceEnd(1999);
constexpr int32_t UserStack(1000);
constexpr int32_t SystemStack(2000);

class CPU {
public:
  enum Status {
    CPURunning = 1 << 0,
    CPUEnding = 1 << 1
  };

  CPU(const int& read_pipe,
      const int& write_pipe,
      const int& timer_trigger)
    : register_pc_(UserSpaceBegin),
      register_sp_(UserStack),
      register_ir_(0),
      register_ac_(0),
      register_x_(0),
      register_y_(0),
      message_(read_pipe, write_pipe),
      instruction_counter_(0),
      mode_(UserMode),
      status_(CPURunning),
      uncalled_timer_(0),
      timer_trigger_(timer_trigger) {
  }

  void FetchNextInstruction(void);
  void ExecuteInstruction(void);
  // TODO: remove register_ir_
  bool IsEnd(void) const {
    return (register_ir_ == 50 || status_ == CPUEnding);
  }

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
  RetValue PullRespond(int32_t& data);

  /*** instructions ***/
  void LoadValue(void); // 1
  RetValue LoadAddr(void); // 2
  void LoadIdxX(void); // 4
  void LoadIdxY(void); // 5
  void LoadSpX(void); // 6
  RetValue Store(void); // 7
  void Put(void); // 9
  void AddX(void); // 10
  void AddY(void); // 11
  void SubX(void); // 12
  void SubY(void); // 13
  void CopyToX(void); // 14
  void CopyFromX(void); // 15
  void CopyToY(void); // 16
  void CopyFromY(void); // 17
  void CopyToSp(void); // 18
  void CopyFromSp(void); // 19
  void Jump(void); // 20
  void JumpIfEqual(void); // 21
  void JumpIfNotEqual(void); // 22
  RetValue CallAddr(void); // 23
  void Ret(void); // 24
  void IncX(void); // 25
  void DecX(void); // 26
  RetValue Push(void); // 27
  void Pop(void); // 28
  RetValue Int(const int32_t& interrupt_address,
           const int32_t& return_address,
           const CPUMode& cpu_mode); // 29
  void IRet(void); // 30
  void End(void); // 50

  // Internal helper
  void LoadIdx(const int32_t& register_);
  void MovePC(const int32_t& offset = 1) {
    register_pc_ += offset;
  }

  // debug helper
  std::string RegisterToString(void);

  void TimerHandler(void);

  uint32_t instruction_counter_;
  CPUMode mode_;
  Status status_;
  uint32_t uncalled_timer_;
  int32_t timer_trigger_;
};

} // namespace vm

#endif // SMPL_MCHN_CPU_H_
