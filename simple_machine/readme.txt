main.cc: main function of program

cpu.cc cpu.h: implementation of class CPU

memory.cc memory.h: implementation of class Memory

message.cc message.h: implementation of class Message

common.h: common structures used by CPU and memory

util.h: logging utility

instruction_table.h: instructions' name table for logging

How to compile:
1. create a build folder outside source folder
2. cmake 'path_to_source'
3. make

How to run:
simple_machine 'path_to_program_file' 'timer_counter'
For example: simple_machine ~/sample5.txt 30
