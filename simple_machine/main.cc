#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "memory.h"
#include "cpu.h"

/*
The parent gets the child's pid returned to him, but the child gets 0 returned to him.
*/

int main(int argc, char* argv[]) {
  pid_t pid;
  int cpu_write_pipe[2]; // aka 0 is for memory_read_pipe
  int memory_write_pipe[2]; // aka 0 is for cpu_read_pipe

  // create two pipes
  if (pipe(cpu_write_pipe) == -1 || pipe(memory_write_pipe) == -1) {
    exit(-1);
  }

  switch (pid = fork()) {
    case -1:
      /* Here pid is -1, the fork failed */
      /* Some possible reasons are that you're */
      /* out of process slots or virtual memory */
      std::cout << "The fork failed!" << std::endl;
      exit(-1);

    case 0: {
      std::cout << "Child: hello!" << std::endl;

      vm::Memory vm_mem(cpu_write_pipe[0], memory_write_pipe[1]);
      vm_mem.Load("/home/christophe/Dropbox/Spring 2016/CS 5348.001 - Operating Systems Concepts/proj_1/sample1.txt");

      do {
        vm_mem.PullRequest();
        vm_mem.DoCommand();
        vm_mem.PushRespond();
      } while(!vm_mem.IsEnd());

      _exit(0);
    }

    default: {
      std::cout << "Parent: child's pid is " << pid << std::endl;

      // power on machine
      vm::CPU vm_cpu(memory_write_pipe[0], cpu_write_pipe[1]);

      do {
        vm_cpu.PushRequest();
        vm_cpu.PullRespond();
        vm_cpu.DoCommand();
      } while(!vm_cpu.IsEnd());

      // TODO: send end message

      waitpid(-1, NULL, 0);
    }
  }

  // TODO: close description
  return 0;
}
