cmake_minimum_required(VERSION 3.14)
project(antlr4-cpp-runtime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)

add_subdirectory(antlr4)