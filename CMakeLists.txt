cmake_minimum_required(VERSION 3.24)
project(bitboards)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(bitboards
        utils.cpp
        utils.h MoveGeneration.h MoveGeneration.cpp)
