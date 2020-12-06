cmake_minimum_required(VERSION 3.14.1)
project(cptoyc)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Basic)
add_subdirectory(Frontend)
add_subdirectory(Lex)
add_subdirectory(llvm)

add_executable(cptoyc main.cpp)

target_link_libraries(cptoyc Basic Lex Frontend LLVMADT)