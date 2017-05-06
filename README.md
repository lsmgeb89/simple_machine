# Simple Machine (Multiple Processes and IPC)

## Summary
  * Implemented a command-line program executing assembly codes with a simplified instruction set
  * Utilized two forked processes to simulate a simple computer system consisting of a CPU and memory
  * Applied a [Unix pipeline][pl] to communicate between the CPU and memory
  * Implemented a simplified instruction set to exhibit important low-level behaviors of an operating system, including:
    1. Processor interaction with main memory
    2. Processor instruction behavior
    3. Role of registers
    4. Stack processing
    5. Procedure calls
    6. System calls
    7. Interrupt handling
    8. Memory protection

## Project Information
  * Course: [Operating Systems Concepts (CS 5348)][os]
  * Professor: [Greg Ozbirn][ozbirn]
  * Semester: Spring 2016
  * Programming Language: C++
  * Build Tool: [CMake][cmake]

[pl]: https://en.wikipedia.org/wiki/Pipeline_(Unix)
[os]: https://catalog.utdallas.edu/2016/graduate/courses/cs5348
[cmake]: https://cmake.org/
[ozbirn]: http://cs.utdallas.edu/people/faculty/ozbirn-greg/
