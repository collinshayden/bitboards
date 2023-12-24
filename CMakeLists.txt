cmake_minimum_required(VERSION 3.24)
project(bitboards)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
add_executable(bitboards
        utils.h
        utils.cpp
        Board.cpp Board.h
        MoveGeneration.cpp MoveGeneration.h
        main.cpp)

#add_executable(bitboards
#        utils.h
#        utils.cpp
#        Board.cpp Board.h
#        MoveGeneration.cpp MoveGeneration.h
#        Tests.h Tests.cpp)