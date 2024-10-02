cmake_minimum_required(VERSION 3.23.2)

# Set project's name and version
project(REmatch VERSION 1.0.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)

# Enable exporting compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use all cores but one for compilation
include(ProcessorCount)
ProcessorCount(NUM_CORES)
if(NUM_CORES GREATER 1)
    math(EXPR PARALLEL_LEVEL "${NUM_CORES} - 1")
    set(CMAKE_BUILD_PARALLEL_LEVEL ${PARALLEL_LEVEL})
endif()

# Define output directories
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Define compiler flags
if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /permissive-")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /RTC1")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL /DNDEBUG")
else()
  # EMSCRIPTEN doesn't support -march=native flag
  if(NOT EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -funroll-loops -fno-operator-names")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -fsanitize=undefined,address -fno-omit-frame-pointer")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g0")
endif()

# Enables PIC on all targets, necessary for building the python library
if(SKBUILD)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  message(STATUS "Build type not specified. Using Release by default.")
else()
  message(STATUS "Using ${CMAKE_BUILD_TYPE} configuration")
endif()

# Enable interprocedural optimization if supported and BUILD_TYPE is RELEASE
string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
if(BUILD_TYPE STREQUAL "RELEASE")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_result OUTPUT ipo_output)
    if(ipo_result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "IPO is not supported:\n${ipo_output}")
    endif(ipo_result)
endif()

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/src/rematch)
# add_subdirectory(${CMAKE_SOURCE_DIR}/src/rematch)

# Define sources
file(GLOB_RECURSE COMMON_SRCS ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE BINARY_SRCS ${CMAKE_SOURCE_DIR}/src/targets/bin/*.cpp)
file(GLOB_RECURSE PYTHON_SRCS ${CMAKE_SOURCE_DIR}/src/targets/python/*.cpp)
file(GLOB_RECURSE JAVASCRIPT_SRCS ${CMAKE_SOURCE_DIR}/src/targets/javascript/*.cpp)
file(GLOB_RECURSE TESTS_SRCS ${CMAKE_SOURCE_DIR}/tests/*.cpp)

# Remove targets from common sources
list(REMOVE_ITEM COMMON_SRCS ${BINARY_SRCS} ${PYTHON_SRCS} ${JAVASCRIPT_SRCS} ${TESTS_SRCS})

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# ANTLR
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/antlr4-cpp-runtime)
include_directories(${CMAKE_SOURCE_DIR}/third_party/antlr4-cpp-runtime/src)

# Define lib
add_library(REmatch-lib STATIC ${COMMON_SRCS})
target_link_libraries(REmatch-lib PRIVATE antlr4_cpp_runtime)

# Define targets
if(SKBUILD)
  include(src/targets/python/CMakeLists.txt)
elseif (EMSCRIPTEN)
  include(src/targets/javascript/CMakeLists.txt)
else()
  include(src/targets/bin/CMakeLists.txt)
endif()

# Enable testing
if(BUILD_TESTING)
  if(EMSCRIPTEN)
    message(WARNING "Skipping tests for javascript target")
  elseif(SKBUILD)
    message(WARNING "Skipping tests for python target")
  else()
    message(STATUS "Building tests")
    add_subdirectory(tests)
  endif()
else()
  message(STATUS "Tests disabled")
endif()

# Enable profiling
if(ENABLE_PROFILING)
  if(EMSCRIPTEN)
    message(WARNING "Skipping profiling for javascript target")
  elseif(SKBUILD)
    message(WARNING "Skipping profiling for python target")
  else()
    message(STATUS "Enable profiling")
    include(cmake/setup_tracy.cmake)
    set(TRACY_ENABLE ON CACHE BOOL "Enable Tracy" FORCE)
    set(TRACY_NO_EXIT ON CACHE BOOL "No Exit Tracy" FORCE)
    set(TRACY_DEBUGINFOD ON CACHE BOOL "Use DEBUGINFOD Tracy" FORCE)
    target_link_libraries(REmatch-lib PUBLIC TracyClient)
  endif()
else()
  message(STATUS "Profiling disabled")
endif()

