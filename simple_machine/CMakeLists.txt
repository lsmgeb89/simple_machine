cmake_minimum_required(VERSION 3.3)
project(simple_machine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES memory.cc main.cc cpu.cc message.cc)
add_executable(simple_machine ${SOURCE_FILES})