# Based on:
# - https://github.com/robotology/how-to-export-cpp-library/
# - https://github.com/cginternals/cmake-init

cmake_minimum_required(VERSION 3.2)

# Set project's name and version
project(REmatch
        LANGUAGES CXX C
        VERSION 1.0.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)

include(CMakePrintHelpers)
include(cmake/utils.cmake)

# Set policies
set_policy(CMP0054 NEW) # ENABLE  CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
set_policy(CMP0042 NEW) # ENABLE  CMP0042: MACOSX_RPATH is enabled by default.
set_policy(CMP0063 NEW) # ENABLE  CMP0063: Honor visibility properties for all target types.
set_policy(CMP0077 NEW) # ENABLE  CMP0077: option() honors normal variables.
set_policy(CMP0120 OLD) # DISABLE CMP0120: The WriteCompilerDetectionHeader module is removed.

# Use all cores but one for compilation
include(ProcessorCount)
ProcessorCount(NUM_CORES)
if(NUM_CORES GREATER 1)
    math(EXPR PARALLEL_LEVEL "${NUM_CORES} - 1")
    set(CMAKE_BUILD_PARALLEL_LEVEL ${PARALLEL_LEVEL})
endif()

# Project options
option(BUILD_SHARED_LIBS "Build shared instead of static libraries." OFF)
option(BUILD_TESTING     "Build tests."                              ON)
option(ENABLE_PROFILING  "Enable profiling"                          OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE                                  ON)

cmake_print_variables(BUILD_TESTING)
cmake_print_variables(ENABLE_PROFILING)

# Under MSVC, we set CMAKE_DEBUG_POSTFIX to "d" to add a trailing "d" to library
# built in debug mode. In this Windows user can compile, build and install the
# library in both Release and Debug configuration avoiding naming clashes in the
# installation directories.
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Check for system dir install
set(SYSTEM_DIR_INSTALL FALSE)
if("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr" OR "${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
    set(SYSTEM_DIR_INSTALL TRUE)
endif()


# Installation paths
if(UNIX AND SYSTEM_DIR_INSTALL)
    # Install into the system (/usr/bin or /usr/local/bin)
    set(INSTALL_ROOT      "share/${PROJECT_NAME}")       # /usr/[local]/share/<PROJECT_NAME>
    set(INSTALL_CMAKE     "share/${PROJECT_NAME}/cmake") # /usr/[local]/share/<PROJECT_NAME>/cmake
    set(INSTALL_BIN       "bin")                         # /usr/[local]/bin
    set(INSTALL_SHARED    "lib")                         # /usr/[local]/lib
    set(INSTALL_LIB       "lib")                         # /usr/[local]/lib
    set(INSTALL_INCLUDE   "include")                     # /usr/[local]/include
else()
    # Install into local directory
    set(INSTALL_ROOT      ".")                           # ./
    set(INSTALL_CMAKE     "cmake")                       # ./cmake
    set(INSTALL_BIN       "bin")                         # ./bin
    set(INSTALL_SHARED    "lib")                         # ./lib
    set(INSTALL_LIB       "lib")                         # ./lib
    set(INSTALL_INCLUDE   "include")                     # ./include
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${INSTALL_BIN})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${INSTALL_LIB})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${INSTALL_LIB})

# Set runtime path
set(CMAKE_SKIP_BUILD_RPATH            FALSE) # Add absolute path to all dependencies for BUILD
set(CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE) # Use CMAKE_INSTALL_RPATH for INSTALL
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE) # Do NOT add path to dependencies for INSTALL

if(NOT SYSTEM_DIR_INSTALL)
    # Find libraries relative to binary
    if(APPLE)
        set(EXECUTABLE_INSTALL_RPATH "@loader_path/../../../${INSTALL_LIB}")
        set(LIBRARY_INSTALL_RPATH    "@loader_path/../../../${INSTALL_LIB}")
    else()
        set(EXECUTABLE_INSTALL_RPATH "$ORIGIN/${INSTALL_LIB}")
        set(LIBRARY_INSTALL_RPATH    "$ORIGIN")
    endif()
endif()


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


# Set default build type as Release if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  message(STATUS "Build type not specified. Using Release by default.")
else()
  message(STATUS "Using ${CMAKE_BUILD_TYPE} configuration")
endif()

# Third parties
add_subdirectory(third_party/antlr4-cpp-runtime)
include_directories(third_party/antlr4-cpp-runtime/src)

include_directories(third_party/cli11)

# Source code
file(GLOB_RECURSE REMATCH_SRC src/rematch/*.cpp)
file(GLOB_RECURSE REMATCH_HDR src/rematch/*.hpp)
include_directories(src/rematch)

### Compile and install related commands.
add_subdirectory(src)