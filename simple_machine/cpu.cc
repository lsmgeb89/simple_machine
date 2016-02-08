#include <iostream>
#include "cpu.h"
#include "instruction_table.h"

namespace vm {

void CPU::PushRequest(const MessagePart& message_part) {
  message_.SetMessage(Request, message_part);
  message_.PushMessage();
}

void CPU::PullRespond(int32_t& data) {
  message_.PullMessage();
  message_.GetRespondData(data);
}

void CPU::FetchNextInstruction(void) {
  PushRequest({ReadMemory, static_cast<MemoryAddress>(register_pc_)});
  PullRespond(register_ir_);
}

void CPU::ExecuteInstruction(void) {
  switch (register_ir_) {
    case 1:
      LoadValue();
      break;
    case 2:
      LoadAddr();
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
    case 11:
      AddY();
      break;
    case 12:
    case 13:
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
    case 19:
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
      Int(SYSTEM_CALL, register_pc_ + 1);
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

  // timer
  if (!(instruction_counter_ % 3) && (mode_ == UserMode)) {
    Int(TIMER_CALL, register_pc_);
  }

  // debug
#if _DEBUG
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
}

// 1
void CPU::LoadValue(void) {
  PushRequest({ReadMemory, static_cast<MemoryAddress>(++register_pc_)});
  PullRespond(register_ac_);
  register_pc_++;
}

// 2
void CPU::LoadAddr(void) {
  // load address
  int32_t address;
  PushRequest({ReadMemory, static_cast<MemoryAddress>(++register_pc_)});
  PullRespond(address);

  // load value at address
  int32_t value;
  PushRequest({ReadMemory, static_cast<MemoryAddress>(address)});
  PullRespond(value);

  register_ac_ = value;
  MovePC();
}

// 7
void CPU::Store(void) {
  int32_t address;
  PushRequest({ReadMemory, static_cast<MemoryAddress>(++register_pc_)});
  PullRespond(address);

  // Store AC into address
  MessagePart message_part;
  Message::SetupWriteMessage(static_cast<MemoryAddress>(address),
                             register_ac_,
                             message_part);
  PushRequest(message_part);
  MovePC();
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
  PushRequest({ReadMemory, static_cast<MemoryAddress>(register_sp_ + register_x_)});
  PullRespond(register_ac_);
  MovePC();
}

void CPU::LoadIdx(const int32_t& register_) {
  int32_t operand;
  PushRequest({ReadMemory, static_cast<MemoryAddress>(++register_pc_)});
  PullRespond(operand);
  PushRequest({ReadMemory, static_cast<MemoryAddress>(operand + register_)});
  PullRespond(register_ac_);
}

// 21
void CPU::JumpIfEqual(void) {
  if (!register_ac_) {
    PushRequest({ReadMemory, static_cast<MemoryAddress>(++register_pc_)});
    PullRespond(register_pc_);
  } else {
    register_pc_ += 2;
  }
}

// 22
void CPU::JumpIfNotEqual(void) {
  if (register_ac_) {
    PushRequest({ReadMemory, static_cast<MemoryAddress>(++register_pc_)});
    PullRespond(register_pc_);
  } else {
    MovePC(2);
  }
}

void CPU::Put(void) {
  int32_t port;
  PushRequest({ReadMemory, static_cast<MemoryAddress>(++register_pc_)});
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
  PushRequest({ReadMemory, static_cast<MemoryAddress>(++register_pc_)});
  PullRespond(register_pc_);
}

void CPU::AddY(void) {
  register_ac_ += register_y_;
  register_pc_++;
}

void CPU::End(void) {
  PushRequest({EndProcess, 0});
}

// 23
void CPU::CallAddr(void) {
  MessagePart message_part;

  // Push return address onto stack
  Message::SetupWriteMessage(static_cast<MemoryAddress>(--register_sp_),
                             register_pc_ + 2,
                             message_part);
  PushRequest(message_part);

  Jump();
}

// 24
void CPU::Ret(void) {
  // Pop return address from the stack
  PushRequest({ReadMemory, static_cast<MemoryAddress>(register_sp_++)});

  // jump to the address
  PullRespond(register_pc_);
}

// 26
void CPU::DecX(void) {
  --register_x_;
  MovePC();
}

// 27
void CPU::Push(void) {
  MessagePart message_part;

  // Push AC onto stack
  Message::SetupWriteMessage(static_cast<MemoryAddress>(--register_sp_),
                             register_ac_,
                             message_part);
  PushRequest(message_part);
  MovePC();
}

// 28
void CPU::Pop(void) {
  PushRequest({ReadMemory, static_cast<MemoryAddress>(register_sp_++)});
  PullRespond(register_ac_);
  MovePC();
}

// 29
void CPU::Int(const int32_t& interrupt_address,
              const int32_t& return_address) {
  // enter Kernel mode
  mode_ = KernelMode;

  int32_t user_sp = register_sp_;
  register_sp_ = SYSTEM_STACK;

  MessagePart message_part;

  // Push User SP onto stack
  Message::SetupWriteMessage(static_cast<MemoryAddress>(--register_sp_),
                             user_sp,
                             message_part);
  PushRequest(message_part);

  // Push User PC onto stack
  Message::SetupWriteMessage(static_cast<MemoryAddress>(--register_sp_),
                             return_address,
                             message_part);
  PushRequest(message_part);

  register_pc_ = interrupt_address;
}

// 30
void CPU::IRet(void) {
  // Pop User PC from stack
  PushRequest({ReadMemory, static_cast<MemoryAddress>(register_sp_++)});
  PullRespond(register_pc_);

  // Pop User SP from stack
  PushRequest({ReadMemory, static_cast<MemoryAddress>(register_sp_++)});
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

} // namespace vm
