#include <iostream>
#include "cpu.h"
#include "instruction_table.h"

namespace vm {

void CPU::PushRequest(const CommandType& command_type,
                      const int32_t& memory_address) {
  msg_ = {Request, {command_type, static_cast<MemoryAddress>(memory_address)}};
  message_.PushMessage(msg_);
}

void CPU::PullRespond(int32_t& data) {
  message_.PullMessage(msg_);
  if (msg_.type_ == Respond) {
    data = msg_.respond_part_.data_;
  } else {
    std::cerr << "[error] wrong type of respond message!" << std::endl;
  }
}

void CPU::FetchNextInstruction(void) {
  PushRequest(ReadMemory, register_pc_);
  PullRespond(register_ir_);
}

void CPU::ExecuteInstruction(void) {
  switch (register_ir_) {
    case 1:
      LoadValue();
      break;
    case 2:
    case 3:
    case 4:
      LoadIdxX();
      break;
    case 5:
      LoadIdxY();
      break;
    case 6:
    case 7:
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
    case 16:
      CopyToY();
      break;
    case 17:
    case 18:
    case 19:
    case 20:
      Jump();
      break;
    case 21:
      JumpIfEqual();
      break;
    case 22:
    case 23:
    case 24:
    case 25:
      IncX();
      break;
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 50:
      End();
      break;
    default:
      break;
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

void CPU::LoadValue(void) {
  PushRequest(ReadMemory, ++register_pc_);
  PullRespond(register_ac_);
  register_pc_++;
}

void CPU::CopyToX(void) {
  register_x_ = register_ac_;
  register_pc_++;
}

void CPU::CopyToY(void) {
  register_y_ = register_ac_;
  register_pc_++;
}

void CPU::LoadIdxX(void) {
  LoadIdx(register_x_);
  register_pc_++;
}

void CPU::LoadIdxY(void) {
  LoadIdx(register_y_);
  register_pc_++;
}

void CPU::LoadIdx(const int32_t& register_) {
  int32_t operand;
  PushRequest(ReadMemory, ++register_pc_);
  PullRespond(operand);
  PushRequest(ReadMemory, operand + register_);
  PullRespond(register_ac_);
}

void CPU::JumpIfEqual(void) {
  if (!register_ac_) {
    PushRequest(ReadMemory, ++register_pc_);
    PullRespond(register_pc_);
  } else {
    register_pc_ += 2;
  }
}

void CPU::Put(void) {
  int32_t port;
  PushRequest(ReadMemory, ++register_pc_);
  PullRespond(port);
  if (port == 1) {
    std::cout << register_ac_;
  } else if (port == 2) {
    std::cout << static_cast<char>(register_ac_);
  }
  register_pc_++;
}

void CPU::IncX(void) {
  ++register_x_;
  register_pc_++;
}

void CPU::Jump(void) {
  int32_t address;
  PushRequest(ReadMemory, ++register_pc_);
  PullRespond(address);
  register_pc_= address;
}

void CPU::AddY(void) {
  register_ac_ += register_y_;
  register_pc_++;
}

void CPU::End(void) {
  PushRequest(EndProcess, 0);
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
