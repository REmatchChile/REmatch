cmake_minimum_required(VERSION 3.23.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(rematch LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(project_LIB ${PROJECT_NAME}lib)
# Use all cores but one for compilation
include(ProcessorCount)
ProcessorCount(NUM_CORES)
if(NUM_CORES GREATER 1)
    math(EXPR PARALLEL_LEVEL "${NUM_CORES} - 1")
    set(CMAKE_BUILD_PARALLEL_LEVEL ${PARALLEL_LEVEL})
endif()

# Enable testing
set(REMATCH_BUILD_TESTING OFF)
# Enable profiling
set(ENABLE_PROFILING OFF CACHE BOOL "Enable Profiling" FORCE)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  message(STATUS "Build type not specified. Using Release by default.")
else()
  message(STATUS "Using ${CMAKE_BUILD_TYPE} configuration")
endif()

if(SKBUILD)
  # Enables PIC for all targets
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g0 -pedantic")
endif()

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")


include_directories(src/rematch)
add_subdirectory(src/rematch)


# Tracy
include(cmake/setup_tracy.cmake)
# ANTLR
add_subdirectory("${CMAKE_SOURCE_DIR}/third_party/antlr4-cpp-runtime")
include_directories("${CMAKE_SOURCE_DIR}/third_party/antlr4-cpp-runtime/src")


include_directories(src/targets)
add_subdirectory(src/targets)

if(REMATCH_BUILD_TESTING)
  if(ENABLE_PROFILING)
    message(STATUS "Skipped testing build due to profiling.")
  elseif(SKBUILD)
    message(STATUS "Skipped testing build due to SKBUILD.")
  elseif(EMSCRIPTEN)
    message(STATUS "Skipped testing build due to EMSCRIPTEN.")
  else()
    add_subdirectory(tests)
  endif()
endif()
