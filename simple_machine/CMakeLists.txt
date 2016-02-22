cmake_minimum_required(VERSION 3.3)

project(simple_machine)

file(GLOB SIMPLE_MACHINE_SOURCES "*.h" "*.cc")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(simple_machine ${SIMPLE_MACHINE_SOURCES})