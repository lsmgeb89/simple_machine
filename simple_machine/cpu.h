#ifndef SIMPLE_MACHINE_CPU_H_
#define SIMPLE_MACHINE_CPU_H_

#include <random>
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
    : mode_(UserMode),
      register_pc_(UserSpaceBegin),
      register_sp_(UserStack),
      register_ir_(0),
      register_ac_(0),
      register_x_(0),
      register_y_(0),
      instruction_counter_(0),
      uncalled_timer_(0),
      timer_trigger_(timer_trigger),
      message_(read_pipe, write_pipe),
      status_(CPURunning),
      dist_(1, 100) {
    random_engine_.seed(std::random_device()());
  }

  void FetchNextInstruction(void);
  void ExecuteInstruction(void);
  void CheckTimer(void);
  bool IsEnd(void) const {
    return (status_ == CPUEnding);
  }

private:
  /*** Hardware stuffs ***/
  CPUMode mode_;

  // registers
  int32_t register_pc_;
  int32_t register_sp_;
  int32_t register_ir_;
  int32_t register_ac_;
  int32_t register_x_;
  int32_t register_y_;

  // timer
  uint32_t instruction_counter_;
  uint32_t uncalled_timer_;
  int32_t timer_trigger_;

  // message for request and response
  Message message_;

  Status status_;

  // random number for Get()
  std::mt19937 random_engine_;
  std::uniform_int_distribution<std::mt19937::result_type> dist_;

  /*** instructions ***/
  RetValue LoadValue(void); // 1
  RetValue LoadAddress(void); // 2
  RetValue LoadIndAddress(void); // 3
  void LoadIdxX(void); // 4
  void LoadIdxY(void); // 5
  RetValue LoadSpX(void); // 6
  RetValue Store(void); // 7
  void Get(void); // 8
  RetValue Put(void); // 9
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
  RetValue Jump(void); // 20
  RetValue JumpIfEqual(void); // 21
  RetValue JumpIfNotEqual(void); // 22
  RetValue CallAddress(void); // 23
  RetValue Ret(void); // 24
  void IncX(void); // 25
  void DecX(void); // 26
  RetValue Push(void); // 27
  RetValue Pop(void); // 28
  RetValue Int(const int32_t& interrupt_address,
               const int32_t& return_address,
               const CPUMode& cpu_mode); // 29
  RetValue IRet(void); // 30
  RetValue Mod(void); // 31
  RetValue Call(void); // 32
  void End(void); // 50

  // internal helpers
  RetValue LoadIdx(const int32_t& register_);
  void MovePC(const int32_t& offset = 1) {
    register_pc_ += offset;
  }

  // pipe operation wrappers
  void PushRequest(const MessagePart& message_part);
  RetValue PullRespond(int32_t& data);

  // debug helpers
  std::string RegisterToString(void) const;
  std::string CPUInfoToString(void) const;
};

} // namespace vm

#endif // SIMPLE_MACHINE_CPU_H_
