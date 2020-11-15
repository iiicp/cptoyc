cmake_minimum_required(VERSION 3.14.1)
project(cptoyc)
set(CMAKE_CXX_STANDARD 17)
add_executable(cptoyc lexer.h lexer.cpp main.cpp parse.cpp parse.h)