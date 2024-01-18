cmake_minimum_required(VERSION 3.23.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(rematch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable testing
set(REMATCH_BUILD_TESTING ON)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  message(STATUS "Build type not specified. Using Release by default.")
endif ()

message(STATUS "Using ${CMAKE_BUILD_TYPE} configuration")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -pthread -O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -pthread -O3 -g0 -pedantic")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

include_directories(SYSTEM src/thirdparty)
include_directories(src/thirdparty/antlr4/runtime/src)
include_directories(src/rematch)

add_subdirectory(src/thirdparty)
add_subdirectory(src/rematch)
add_subdirectory(src/targets)

if (REMATCH_BUILD_TESTING)
  add_subdirectory(tests)
endif (REMATCH_BUILD_TESTING)
