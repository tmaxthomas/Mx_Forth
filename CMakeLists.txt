cmake_minimum_required(VERSION 3.6)
project(Mx_Forth)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Stack.cpp Stack.h)
add_executable(Mx_Forth ${SOURCE_FILES})