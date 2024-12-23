cmake_minimum_required(VERSION 3.16)
project(hello-rematch) 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(REmatch REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main REmatch::REmatch)