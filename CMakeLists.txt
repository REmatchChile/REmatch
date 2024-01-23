cmake_minimum_required(VERSION 3.23.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(rematch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable testing
set(REMATCH_BUILD_TESTING OFF)

# Enable profiling
set(ENABLE_PROFILING OFF CACHE BOOL "Enable Profiling" FORCE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  message(STATUS "Build type not specified. Using Release by default.")
endif()

message(STATUS "Using ${CMAKE_BUILD_TYPE} configuration")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -O3 -g0 -pedantic")

if(EMSCRIPTEN)
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwasm-exceptions")
endif()

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

include_directories(SYSTEM src/thirdparty)
include_directories(src/thirdparty/antlr4/runtime/src)
include_directories(src/rematch)

add_subdirectory(src/thirdparty)
include(cmake/setup_tracy.cmake)
add_subdirectory(src/rematch)
add_subdirectory(src/targets)

if(REMATCH_BUILD_TESTING)
  if(ENABLE_PROFILING)
    message(STATUS "Skipped testing build due to profiling.")
  else()
    add_subdirectory(tests)
  endif()
endif()
