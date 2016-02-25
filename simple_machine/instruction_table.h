#ifndef SIMPLE_MACHINE_INSTRUCTION_TABLE_H_
#define SIMPLE_MACHINE_INSTRUCTION_TABLE_H_

#include <vector>

namespace vm {

// all supported instructions
std::vector<std::string> instruction_table = {
  "NULL",
  "LOAD VALUE",
  "LOAD ADDR",
  "LOADIND ADDR",
  "LOADIDXX ADDR",
  "LOADIDXY ADDR",
  "LOADSPX",
  "STORE ADDR",
  "GET",
  "PUT PORT",
  "ADDX",
  "ADDY",
  "SUBX",
  "SUBY",
  "COPYTOX",
  "COPYFROMX",
  "COPYTOY",
  "COPYFROMY",
  "COPYTOSP",
  "COPYFROMSP",
  "JUMP ADDR",
  "JUMPIFEQUAL ADDR",
  "JUMPIFNOTEQUAL ADDR",
  "CALL ADDR",
  "RET",
  "INCX",
  "DECX",
  "PUSH",
  "POP",
  "INT",
  "IRET",
  "MOD",
  "CALL"
};

} // namespace vm

#endif //SIMPLE_MACHINE_INSTRUCTION_TABLE_H_
