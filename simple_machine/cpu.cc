#include "cpu.h"

namespace vm {

void CPU::PushRequest(void) {
  MessageContent msg = {Request, {register_pc_, Read}};
  message_.PushMessage(msg);
}

void CPU::PullRespond(void) {

}

void CPU::DoCommand(void) {

}

bool CPU::IsEnd() {
  return false;
}

} // namespace vm
