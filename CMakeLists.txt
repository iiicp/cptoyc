cmake_minimum_required(VERSION 3.14.1)
project(cptoyc)
set(CMAKE_CXX_STANDARD 17)

file(GLOB Basic Basic/*.c*)
file(GLOB llvm llvm/[^.]*.c*)
file(GLOB Lex Lex/[^.]*.c*)
file(GLOB Frontend Frontend/[^.]*.c*)

#include_directories(Basic Lex llvm Frontend)

add_executable(cptoyc main.cpp ${Basic} ${llvm} ${Lex} ${Frontend})