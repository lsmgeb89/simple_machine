#ifndef SIMPLE_MACHINE_COMMON_H
#define SIMPLE_MACHINE_COMMON_H

namespace vm {

enum CPUMode {
  UserMode = 1 << 0,
  TimerMode = 1 << 1,
  SystemMode = 1 << 2
};

enum RetValue {
  Success = 0,
  MemoryViolation = 1 << 0,
  MemoryError = 1 << 1,
  WrongMessageType = 1 << 2,
  FileNotExisted = 1 << 3,
  InvalidProgramFile = 1 << 4
};

#define IsSuccess(func) ((func) == Success)
#define Check(ret, func)        \
if (!IsSuccess(ret = (func))) { \
  goto done;                    \
}                               \

} // namespace vm

#endif // SIMPLE_MACHINE_COMMON_H
