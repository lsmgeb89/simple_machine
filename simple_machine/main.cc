#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "memory.h"

int main(int argc, char* argv[]) {
  pid_t pid, res;
  int status;
  int cpu_write_pipe[2]; // aka 0 is for memory_read_pipe
  int memory_write_pipe[2]; // aka 0 is for cpu_read_pipe

  // options
  std::string file_path;
  int32_t interrupt_timer(0);

  // parse commandline
  if (argc == 3 &&
      (access(argv[1], R_OK) != -1) &&
      (std::stoi(argv[2]) > 0)) {
    file_path = argv[1];
    interrupt_timer = std::stoi(argv[2]);
  } else {
    error_system << "Wrong Parameter" << std::endl;
    exit(0);
  }

  // create two pipes
  if (pipe(cpu_write_pipe) == -1 ||
      pipe(memory_write_pipe) == -1) {
    perror(LOG_ERROR_MODULE_SYSTEM"[pipe]");
    exit(-1);
  }

  switch (pid = fork()) {
    case -1:
      perror("LOG_ERROR_MODULE_SYSTEM[fork]");
      exit(-1);

    case 0: {
      info_system << "Child process created." << std::endl;

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

      info_system << "Child process is ending." << std::endl;
      close(cpu_write_pipe[0]); // Close read end
      close(memory_write_pipe[1]); // Close write end

      exit(0);
    }

    default: {
      info_system << "Parent process created and child's pid is " << pid << std::endl;

      close(cpu_write_pipe[0]); // Close unused read end
      close(memory_write_pipe[1]); // Close unused write end

      vm::CPU vm_cpu(memory_write_pipe[0],
                     cpu_write_pipe[1],
                     interrupt_timer);

      do {
        vm_cpu.FetchNextInstruction();
        vm_cpu.ExecuteInstruction();
        vm_cpu.CheckTimer();
      } while(!vm_cpu.IsEnd());

      close(cpu_write_pipe[1]); // Close write end
      close(memory_write_pipe[0]); // Close read end

      res = waitpid(-1, &status, 0);
      if (res == -1) {
        perror("LOG_ERROR_MODULE_SYSTEM[waitpid]");
        exit(-1);
      }

      if (WIFEXITED(status)) {
        info_system << "Exited, status = " << WEXITSTATUS(status) << std::endl;
      } else if (WIFSIGNALED(status)) {
        info_system << "killed by signal: " << WTERMSIG(status) << std::endl;
      } else if (WIFSTOPPED(status)) {
        info_system << "Stopped by signal: " << WSTOPSIG(status) << std::endl;
      } else if (WIFCONTINUED(status)) {
        info_system << "Continued" << std::endl;
      }
    }
  }

  return 0;
}
