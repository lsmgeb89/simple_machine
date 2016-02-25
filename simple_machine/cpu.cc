#include <iostream>
#include "cpu.h"
#include "instruction_table.h"

namespace vm {

void CPU::FetchNextInstruction(void) {
  PushRequest({ReadMemory, mode_, register_pc_});
  PullRespond(register_ir_);
}

void CPU::ExecuteInstruction(void) {
  RetValue ret(Success);

  if (IsEnd()) { return; }

  switch (register_ir_) {
    case 1:
      ret = LoadValue();
      break;
    case 2:
      ret = LoadAddress();
      break;
    case 3:
      ret = LoadIndAddress();
      break;
    case 4:
      LoadIdxX();
      break;
    case 5:
      LoadIdxY();
      break;
    case 6:
      ret = LoadSpX();
      break;
    case 7:
      ret = Store();
      break;
    case 8:
      Get();
      break;
    case 9:
      ret = Put();
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
      ret = Jump();
      break;
    case 21:
      ret = JumpIfEqual();
      break;
    case 22:
      ret = JumpIfNotEqual();
      break;
    case 23:
      ret = CallAddress();
      break;
    case 24:
      ret = Ret();
      break;
    case 25:
      IncX();
      break;
    case 26:
      DecX();
      break;
    case 27:
      ret = Push();
      break;
    case 28:
      ret = Pop();
      break;
    case 29:
      ret = Int(SystemSpaceBegin, register_pc_ + 1, SystemMode);
      break;
    case 30:
      ret = IRet();
      break;
    case 31:
      ret = Mod();
      break;
    case 32:
      ret = Call();
      break;
    case 50:
      End();
      break;
    default:
      break;
  }

  if (!IsSuccess(ret)) { return; }

  ++instruction_counter_;
  info_cpu << CPUInfoToString() << std::endl;
}

void CPU::CheckTimer(void) {
  if (IsEnd()) { return; }

  // delay triggered timer
  if (mode_ == UserMode && uncalled_timer_ > 0) {
    info_cpu << "[Timer] Trigger delayed Timer @ instruction " << instruction_counter_ << std::endl;
    Int(TimerSpaceBegin, register_pc_, TimerMode);
    --uncalled_timer_;
  }

  // timer
  if (!(instruction_counter_ % timer_trigger_)) {
    if (mode_ == UserMode) {
      info_cpu << "[Timer] Trigger Timer @ instruction " << instruction_counter_ << std::endl;
      Int(TimerSpaceBegin, register_pc_, TimerMode);
    } else {
      // add delay counter
      info_cpu << "[Timer] Delay Timer @ instruction " << instruction_counter_ << std::endl;
      uncalled_timer_++;
    }
  }
}

// 1
RetValue CPU::LoadValue(void) {
  RetValue ret(Success);

  PushRequest({ReadMemory, mode_, ++register_pc_});
  Check(ret, PullRespond(register_ac_))

  MovePC();

done:
  return ret;
}

// 2
RetValue CPU::LoadAddress(void) {
  RetValue ret(Success);

  // load address
  int32_t address;
  PushRequest({ReadMemory, mode_, ++register_pc_});
  Check(ret, PullRespond(address))

  // load value at address
  int32_t value;
  PushRequest({ReadMemory, mode_, address});
  Check(ret, PullRespond(value))

  register_ac_ = value;
  MovePC();

done:
  return ret;
}

// 3
RetValue CPU::LoadIndAddress(void) {
  RetValue ret(Success);

  // fetch operand
  int32_t operand;
  PushRequest({ReadMemory, mode_, ++register_pc_});
  Check(ret, PullRespond(operand))

  // fetch address
  int32_t address;
  PushRequest({ReadMemory, mode_, operand});
  Check(ret, PullRespond(address))

  // fetch value
  PushRequest({ReadMemory, mode_, address});
  Check(ret, PullRespond(register_ac_))

  MovePC();

done:
  return ret;
}

// 4
void CPU::LoadIdxX(void) {
  LoadIdx(register_x_);
  MovePC();
}

// 5
void CPU::LoadIdxY(void) {
  LoadIdx(register_y_);
  MovePC();
}

// 6
RetValue CPU::LoadSpX(void) {
  RetValue ret(Success);

  PushRequest({ReadMemory, mode_, (register_sp_ + register_x_)});
  Check(ret, PullRespond(register_ac_))
  MovePC();

  done:
  return ret;
}

// 7
RetValue CPU::Store(void) {
  RetValue ret(Success);

  int32_t address;
  PushRequest({ReadMemory, mode_, ++register_pc_});
  Check(ret, PullRespond(address))

  // Store AC into address
  MessagePart message_part;
  Message::SetupWriteMessage(address,
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

// 8
void CPU::Get(void) {
  register_ac_ = static_cast<int32_t>(dist_(random_engine_));
  MovePC();
}

// 9
RetValue CPU::Put(void) {
  RetValue ret(Success);
  int32_t port;

  PushRequest({ReadMemory, mode_, ++register_pc_});
  Check(ret, PullRespond(port))

  if (port == 1) {
    std::cerr << register_ac_;
  } else if (port == 2) {
    std::cerr << static_cast<char>(register_ac_);
  }

  MovePC();

  done:
  return ret;
}

// 10
void CPU::AddX(void) {
  register_ac_ += register_x_;
  MovePC();
}

// 11
void CPU::AddY(void) {
  register_ac_ += register_y_;
  MovePC();
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

// 14
void CPU::CopyToX(void) {
  register_x_ = register_ac_;
  MovePC();
}

// 15
void CPU::CopyFromX(void) {
  register_ac_ = register_x_;
  MovePC();
}

// 16
void CPU::CopyToY(void) {
  register_y_ = register_ac_;
  MovePC();
}

// 17
void CPU::CopyFromY(void) {
  register_ac_ = register_y_;
  MovePC();
}

// 18
void CPU::CopyToSp(void) {
  register_sp_ = register_ac_;
  MovePC();
}

// 19
void CPU::CopyFromSp(void) {
  register_ac_ = register_sp_;
  MovePC();
}

// 20
RetValue CPU::Jump(void) {
  RetValue ret(Success);

  PushRequest({ReadMemory, mode_, ++register_pc_});
  Check(ret, PullRespond(register_pc_))

  done:
  return ret;
}

// 21
RetValue CPU::JumpIfEqual(void) {
  RetValue ret(Success);

  if (!register_ac_) {
    PushRequest({ReadMemory, mode_, ++register_pc_});
    Check(ret, PullRespond(register_pc_))
  } else {
    MovePC(2);
  }

done:
  return ret;
}

// 22
RetValue CPU::JumpIfNotEqual(void) {
  RetValue ret(Success);

  if (register_ac_) {
    PushRequest({ReadMemory, mode_, ++register_pc_});
    Check(ret, PullRespond(register_pc_))
  } else {
    MovePC(2);
  }

done:
  return ret;
}

// 23
RetValue CPU::CallAddress(void) {
  RetValue ret(Success);
  MessagePart message_part;

  // Push return address onto stack
  Message::SetupWriteMessage(--register_sp_,
                             register_pc_ + 2,
                             mode_,
                             message_part);
  PushRequest(message_part);

  int32_t value;
  Check(ret, PullRespond(value))

  Check(ret, Jump())

  done:
  return ret;
}

// 24
RetValue CPU::Ret(void) {
  RetValue ret(Success);

  // Pop return address from the stack
  PushRequest({ReadMemory, mode_, register_sp_++});
  // jump to the address
  Check(ret, PullRespond(register_pc_))

  done:
  return ret;
}

// 25
void CPU::IncX(void) {
  ++register_x_;
  MovePC();
}

// 26
void CPU::DecX(void) {
  --register_x_;
  MovePC();
}

// 27
RetValue CPU::Push(void) {
  RetValue ret(Success);
  MessagePart message_part;

  // Push AC onto stack
  Message::SetupWriteMessage(--register_sp_,
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
RetValue CPU::Pop(void) {
  RetValue ret(Success);

  PushRequest({ReadMemory, mode_, register_sp_++});
  Check(ret, PullRespond(register_ac_))

  MovePC();

done:
  return ret;
}

// 29
RetValue CPU::Int(const int32_t& interrupt_address,
                  const int32_t& return_address,
                  const CPUMode& cpu_mode) {
  RetValue ret(Success);
  // enter Kernel mode
  mode_ = cpu_mode;

  int32_t user_sp = register_sp_;
  register_sp_ = SystemStack;

  MessagePart message_part;

  // Push User SP onto stack
  Message::SetupWriteMessage(--register_sp_,
                             user_sp,
                             mode_,
                             message_part);
  PushRequest(message_part);

  int32_t value;
  Check(ret, PullRespond(value))

  // Push User PC onto stack
  Message::SetupWriteMessage(--register_sp_,
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
RetValue CPU::IRet(void) {
  RetValue ret(Success);

  // Pop User PC from stack
  PushRequest({ReadMemory, mode_, register_sp_++});
  Check(ret, PullRespond(register_pc_))

  // Pop User SP from stack
  PushRequest({ReadMemory, mode_, register_sp_++});
  Check(ret, PullRespond(register_sp_))

  // back to user mode
  mode_ = UserMode;

done:
  return ret;
}

// 31
RetValue CPU::Mod(void) {
  RetValue ret(Success);
  int32_t operand;

  PushRequest({ReadMemory, mode_, ++register_pc_});
  Check(ret, PullRespond(operand))

  register_ac_ %= operand;

  MovePC();

done:
  return ret;
}

// 32
RetValue CPU::Call(void) {
  RetValue ret(Success);
  MessagePart message_part;

  // Push return address onto stack
  Message::SetupWriteMessage(--register_sp_,
                             register_pc_ + 1,
                             mode_,
                             message_part);
  PushRequest(message_part);

  int32_t value;
  Check(ret, PullRespond(value))

  register_pc_ = register_ac_;

done:
  return ret;
}

// 50
void CPU::End(void) {
  status_ = CPUEnding;
  PushRequest({EndProcess, mode_, 0});
}

RetValue CPU::LoadIdx(const int32_t& register_) {
  RetValue ret(Success);
  int32_t operand;

  PushRequest({ReadMemory, mode_, ++register_pc_});
  Check(ret, PullRespond(operand))
  PushRequest({ReadMemory, mode_, (operand + register_)});
  Check(ret, PullRespond(register_ac_))

  done:
  return ret;
}

void CPU::PushRequest(const MessagePart& message_part) {
  message_.SetMessage(Request, message_part);
  message_.PushMessage();
}

RetValue CPU::PullRespond(int32_t& data) {
  message_.PullMessage();
  RetValue ret = message_.GetRespondData(data);
  if (MemoryViolation == ret || MemoryError == ret) {
    End();
  }
  return ret;
}

std::string CPU::RegisterToString(void) const {
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

std::string CPU::CPUInfoToString(void) const {
  std::string res;
  res += "(";
  res += std::to_string(instruction_counter_);
  res += ")[";
  res += std::to_string(register_ir_);
  res += "][";
  if (register_ir_ == 50) {
    res += "END";
  } else {
    res += instruction_table[register_ir_];
  }
  res += "]";
  res += RegisterToString();
  return res;
}

} // namespace vm
