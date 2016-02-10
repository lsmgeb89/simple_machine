#include <iostream>
#include "cpu.h"
#include "instruction_table.h"

namespace vm {

void CPU::PushRequest(const MessagePart& message_part) {
  message_.SetMessage(Request, message_part);
  message_.PushMessage();
}

RetValue CPU::PullRespond(int32_t& data) {
  message_.PullMessage();
  RetValue ret = message_.GetRespondData(data);
  if (ret == MemoryViolation) {
    End();
  }
  return ret;
}

void CPU::FetchNextInstruction(void) {
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(register_pc_)});
  PullRespond(register_ir_);
}

void CPU::ExecuteInstruction(void) {
  switch (register_ir_) {
    case 1:
      LoadValue();
      break;
    case 2:
      if (!IsSuccess(LoadAddr())) {
        return;
      }
      break;
    case 3:
    case 4:
      LoadIdxX();
      break;
    case 5:
      LoadIdxY();
      break;
    case 6:
      LoadSpX();
      break;
    case 7:
      Store();
      break;
    case 8:
    case 9:
      Put();
      break;
    case 10:
      AddX();
      break;
    case 11:
      AddY();
      break;
    case 12:
      SubX();
      break;
    case 13:
      SubY();
      break;
    case 14:
      CopyToX();
      break;
    case 15:
      CopyFromX();
      break;
    case 16:
      CopyToY();
      break;
    case 17:
      CopyFromY();
      break;
    case 18:
      CopyToSp();
      break;
    case 19:
      CopyFromSp();
      break;
    case 20:
      Jump();
      break;
    case 21:
      JumpIfEqual();
      break;
    case 22:
      JumpIfNotEqual();
      break;
    case 23:
      CallAddr();
      break;
    case 24:
      Ret();
      break;
    case 25:
      IncX();
      break;
    case 26:
      DecX();
      break;
    case 27:
      Push();
      break;
    case 28:
      Pop();
      break;
    case 29:
      Int(SystemSpaceBegin, register_pc_ + 1, SystemMode);
      break;
    case 30:
      IRet();
      break;
    case 50:
      End();
      break;
    default:
      break;
  }

  ++instruction_counter_;

  // debug
#if _DEBUG
  std::clog << "(" << instruction_counter_ << ")";
  std::clog << "[" << register_ir_ << "]";
  std::clog << "[";
  if (register_ir_ == 50) {
    std::clog << "END";
  } else {
    std::clog << instruction_table[register_ir_];
  }
  std::clog << "]";
  std::clog << RegisterToString() << std::endl;
#endif

  TimerHandler();
}

// 1
void CPU::LoadValue(void) {
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(++register_pc_)});
  PullRespond(register_ac_);
  register_pc_++;
}

// 2
RetValue CPU::LoadAddr(void) {
  RetValue ret;

  // load address
  int32_t address;
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(++register_pc_)});
  Check(ret, PullRespond(address))

  // load value at address
  int32_t value;
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(address)});
  Check(ret, PullRespond(value))

  register_ac_ = value;
  MovePC();

  done:
    return ret;
}

// 7
RetValue CPU::Store(void) {
  RetValue ret;

  int32_t address;
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(++register_pc_)});
  PullRespond(address);

  // Store AC into address
  MessagePart message_part;
  Message::SetupWriteMessage(static_cast<MemoryAddress>(address),
                             register_ac_,
                             mode_,
                             message_part);
  PushRequest(message_part);

  int32_t value;
  Check(ret, PullRespond(value))

  MovePC();
  done:
    return ret;
}

// 14
void CPU::CopyToX(void) {
  register_x_ = register_ac_;
  register_pc_++;
}

// 15
void CPU::CopyFromX(void) {
  register_ac_ = register_x_;
  MovePC();
}

// 16
void CPU::CopyToY(void) {
  register_y_ = register_ac_;
  register_pc_++;
}

// 17
void CPU::CopyFromY(void) {
  register_ac_ = register_y_;
  MovePC();
}

// 4
void CPU::LoadIdxX(void) {
  LoadIdx(register_x_);
  register_pc_++;
}

// 5
void CPU::LoadIdxY(void) {
  LoadIdx(register_y_);
  register_pc_++;
}

// 6
void CPU::LoadSpX(void) {
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(register_sp_ + register_x_)});
  PullRespond(register_ac_);
  MovePC();
}

void CPU::LoadIdx(const int32_t& register_) {
  int32_t operand;
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(++register_pc_)});
  PullRespond(operand);
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(operand + register_)});
  PullRespond(register_ac_);
}

// 18 Untest
void CPU::CopyToSp(void) {
  register_sp_ = register_ac_;
  MovePC();
}

// 19
void CPU::CopyFromSp(void) {
  register_ac_ = register_sp_;
  MovePC();
}

// 21
void CPU::JumpIfEqual(void) {
  if (!register_ac_) {
    PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(++register_pc_)});
    PullRespond(register_pc_);
  } else {
    register_pc_ += 2;
  }
}

// 22
void CPU::JumpIfNotEqual(void) {
  if (register_ac_) {
    PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(++register_pc_)});
    PullRespond(register_pc_);
  } else {
    MovePC(2);
  }
}

void CPU::Put(void) {
  int32_t port;
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(++register_pc_)});
  PullRespond(port);
  if (port == 1) {
    std::cerr << register_ac_;
  } else if (port == 2) {
    std::cerr << static_cast<char>(register_ac_);
  }
  register_pc_++;
}

void CPU::IncX(void) {
  ++register_x_;
  register_pc_++;
}

// 20
void CPU::Jump(void) {
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(++register_pc_)});
  PullRespond(register_pc_);
}

// 10
void CPU::AddX(void) {
  register_ac_ += register_x_;
  MovePC();
}

// 11
void CPU::AddY(void) {
  register_ac_ += register_y_;
  register_pc_++;
}

// 12
void CPU::SubX(void) {
  register_ac_ -= register_x_;
  MovePC();
}

// 13
void CPU::SubY(void) {
  register_ac_ -= register_y_;
  MovePC();
}

// 50
void CPU::End(void) {
  status_ = CPUEnding;
  PushRequest({EndProcess, mode_, 0});
}

// 23
RetValue CPU::CallAddr(void) {
  RetValue ret;
  MessagePart message_part;

  // Push return address onto stack
  Message::SetupWriteMessage(static_cast<MemoryAddress>(--register_sp_),
                             register_pc_ + 2,
                             mode_,
                             message_part);
  PushRequest(message_part);

  int32_t value;
  Check(ret, PullRespond(value))

  Jump();
  done:
    return ret;
}

// 24
void CPU::Ret(void) {
  // Pop return address from the stack
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(register_sp_++)});

  // jump to the address
  PullRespond(register_pc_);
}

// 26
void CPU::DecX(void) {
  --register_x_;
  MovePC();
}

// 27
RetValue CPU::Push(void) {
  RetValue ret;
  MessagePart message_part;

  // Push AC onto stack
  Message::SetupWriteMessage(static_cast<MemoryAddress>(--register_sp_),
                             register_ac_,
                             mode_,
                             message_part);
  PushRequest(message_part);

  int32_t value;
  Check(ret, PullRespond(value))

  MovePC();

  done:
    return ret;
}

// 28
void CPU::Pop(void) {
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(register_sp_++)});
  PullRespond(register_ac_);
  MovePC();
}

// 29
RetValue CPU::Int(const int32_t& interrupt_address,
              const int32_t& return_address,
              const CPUMode& cpu_mode) {
  RetValue ret;
  // enter Kernel mode
  mode_ = cpu_mode;

  int32_t user_sp = register_sp_;
  register_sp_ = SystemStack;

  MessagePart message_part;

  // Push User SP onto stack
  Message::SetupWriteMessage(static_cast<MemoryAddress>(--register_sp_),
                             user_sp,
                             mode_,
                             message_part);
  PushRequest(message_part);

  int32_t value;
  Check(ret, PullRespond(value))

  // Push User PC onto stack
  Message::SetupWriteMessage(static_cast<MemoryAddress>(--register_sp_),
                             return_address,
                             mode_,
                             message_part);
  PushRequest(message_part);

  Check(ret, PullRespond(value))

  register_pc_ = interrupt_address;

  done:
    return ret;
}

// 30
void CPU::IRet(void) {
  // Pop User PC from stack
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(register_sp_++)});
  PullRespond(register_pc_);

  // Pop User SP from stack
  PushRequest({ReadMemory, mode_, static_cast<MemoryAddress>(register_sp_++)});
  PullRespond(register_sp_);

  // back to user mode
  mode_ = UserMode;
}

std::string CPU::RegisterToString(void) {
  std::string res;
  res += " pc: ";
  res += std::to_string(register_pc_);
  res += " ac: ";
  res += std::to_string(register_ac_);
  res += " x: ";
  res += std::to_string(register_x_);
  res += " y: ";
  res += std::to_string(register_y_);
  res += " ir: ";
  res += std::to_string(register_ir_);
  res += " sp: ";
  res += std::to_string(register_sp_);
  return res;
}

void CPU::TimerHandler(void) {
  if (status_ == CPUEnding) {
    return;
  }

  // delay triggered timer
  if (mode_ == UserMode && uncalled_timer_ > 0) {
#ifdef _DEBUG
    std::clog << "[Timer] Enter Delay Timer Interrupt @ instruction " << instruction_counter_ << std::endl;
#endif
    Int(TimerSpaceBegin, register_pc_, TimerMode);
    --uncalled_timer_;
  }

  // timer
  if (!(instruction_counter_ % 30)) {
    if (mode_ == UserMode) {
#ifdef _DEBUG
      std::clog << "[Timer] Enter Timer Interrupt @ instruction " << instruction_counter_ << std::endl;
#endif
      Int(TimerSpaceBegin, register_pc_, TimerMode);
    } else {
      // add delay counter
#ifdef _DEBUG
      std::clog << "[Timer] Delay Timer Interrupt @ instruction " << instruction_counter_ << std::endl;
#endif
      uncalled_timer_++;
    }
  }
}

} // namespace vm
