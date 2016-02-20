#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "memory.h"

/*
The parent gets the child's pid returned to him, but the child gets 0 returned to him.
*/

int main(int argc, char* argv[]) {
  pid_t pid, res;
  int status;
  int cpu_write_pipe[2]; // aka 0 is for memory_read_pipe
  int memory_write_pipe[2]; // aka 0 is for cpu_read_pipe

  // options
  std::string file_path;
  int32_t interrupt_timer(0);

  // parse commandline
  if (argc == 3) {
    file_path = argv[1];
    interrupt_timer = atoi(argv[2]);
  } else {
    std::cout << "!!! WRONG PARAMETER !!!" << std::endl;
    exit(0);
  }

  // create two pipes
  if (pipe(cpu_write_pipe) == -1 || pipe(memory_write_pipe) == -1) {
    exit(-1);
  }

  switch (pid = fork()) {
    case -1:
      /* Here pid is -1, the fork failed */
      /* Some possible reasons are that you're */
      /* out of process slots or virtual memory */
      std::cerr << "The fork failed!" << std::endl;
      exit(-1);

    case 0: {
      std::cout << "Child: hello!" << std::endl;

      close(cpu_write_pipe[1]); // Close unused write end
      close(memory_write_pipe[0]); // Close unused read end

      vm::Memory vm_mem(cpu_write_pipe[0],
                        memory_write_pipe[1],
                        file_path);
      vm_mem.Init();

      do {
        vm_mem.PullRequest();
        vm_mem.PrepareRespond();
        vm_mem.PushRespond();
      } while(!vm_mem.IsEnd());

      std::clog << "Child process end!" << std::endl;
      close(cpu_write_pipe[0]); // Close read end
      close(memory_write_pipe[1]); // Close write end

      _exit(0);
    }

    default: {
      std::cout << "Parent: child's pid is " << pid << std::endl;

      close(cpu_write_pipe[0]); // Close unused read end
      close(memory_write_pipe[1]); // Close unused write end

      vm::CPU vm_cpu(memory_write_pipe[0],
                     cpu_write_pipe[1],
                     interrupt_timer);

      do {
        vm_cpu.FetchNextInstruction();
        vm_cpu.ExecuteInstruction();
      } while(!vm_cpu.IsEnd());

      close(cpu_write_pipe[1]); // Close write end
      close(memory_write_pipe[0]); // Close read end

      res = waitpid(-1, nullptr, 0);
      if (res == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
      }

      if (WIFEXITED(status)) {
        std::clog << "exited, status = " << WEXITSTATUS(status) << std::endl;
      } else if (WIFSIGNALED(status)) {
        std::clog << "killed by signal: " << WTERMSIG(status) << std::endl;
      } else if (WIFSTOPPED(status)) {
        std::clog << "stopped by signal: " << WSTOPSIG(status) << std::endl;
      } else if (WIFCONTINUED(status)) {
        std::clog << "continued" << std::endl;
      }
    }
  }

  return 0;
}
