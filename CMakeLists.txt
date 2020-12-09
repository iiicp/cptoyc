cmake_minimum_required(VERSION 3.14.1)
project(cptoyc)
set(CMAKE_CXX_STANDARD 17)

file(GLOB Basic Basic/*.c*)
file(GLOB llvm llvm/[^.]*.c*)
file(GLOB Lex Lex/[^.]*.c*)
file(GLOB Frontend Frontend/[^.]*.c*)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/Basic
        ${PROJECT_SOURCE_DIR}/Lex
        ${PROJECT_SOURCE_DIR}/llvm
        ${PROJECT_SOURCE_DIR}/Frontend)

add_executable(cptoyc main.cpp ${Basic} ${llvm} ${Lex} ${Frontend})