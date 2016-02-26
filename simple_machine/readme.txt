--src
   |
   main.cc: main entrance of program
   |
   cpu.cc cpu.h: implementation of class CPU
   |
   memory.cc memory.h: implementation of class Memory
   |
   message.cc message.h: implementation of class Message
   |
   common.h: common structures used by CPU and memory
   |
   util.h: logging utility
   |
   instruction_table.h: instructions' name table for logging
   |
   CMakeLists.txt: CMake file for building project

--test_cases
   |
   sample1.txt sample2.txt sample3.txt sample4.txt: teacher's test cases
   |
   sample5.txt: shortest NetID password generator

summary.pdf: summary document

How to compile (only support on csgrads1.utdallas.edu by using Linux shell command):
1. create a build folder outside the src folder
   (eg: mkdir build_minsizerel)
2. change directory to the build folder
   (eg: cd build_minsizerel)
2. cmake 'path_to_source_root' -DCMAKE_BUILD_TYPE=MINSIZEREL
   (eg: cmake ../src -DCMAKE_BUILD_TYPE=MINSIZEREL)
3. make

How to run:
simple_machine 'path_to_program_file' 'timer_counter'
(eg: simple_machine ~/sample5.txt 30)
