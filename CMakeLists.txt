set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


cmake_minimum_required(VERSION 3.23.2)

project(rematch LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    message(STATUS "Build type not specified. Using Release by default.")
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable testing
set(REMATCH_BUILD_TESTING ON)
# Enable benchmarks
set(REMATCH_BUILD_BENCHMARKS ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pthread -O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -pthread -O3 -g0 -pedantic")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

include(FetchContent)

# Profiler
message("Fetching Tracy")

set(ENABLE_PROFILING OFF CACHE BOOL "Enable Profiling" FORCE)

Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare (
  tracy
  GIT_REPOSITORY https://github.com/wolfpld/tracy.git
  GIT_TAG master
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)

if (ENABLE_PROFILING)
  set(TRACY_ENABLE ON CACHE BOOL "Enable Tracy" FORCE)
  set(TRACY_NO_EXIT ON CACHE BOOL "No Exit Tracy" FORCE)
  set(TRACY_DEBUGINFOD ON CACHE BOOL "Use DEBUGINFOD Tracy" FORCE)
else()
  set(TRACY_ENABLE OFF CACHE BOOL "Disable Tracy" FORCE)
endif()

FetchContent_MakeAvailable (tracy)
target_compile_options(TracyClient PRIVATE -w)

#
add_compile_definitions(
  BENCHMARK_MODE
  # NOPT_MEMORY
  # NOPT_ASCIIARRAY
  # NOPT_OFFSET
  # NOPT_EARLYOUTPUT
  # NOPT_SEGMENT_RATIO
)

# Rematch libs starts as empty list
set(REMATCH_LIBS "")
list(APPEND REMATCH_LIBS TracyClient)

include_directories(src/rematch)
include_directories(SYSTEM src/thirdparty)
include_directories(src/thirdparty/antlr4/runtime/src)


# Maybe remove boost
find_package(Boost 1.58)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
# No need to add Boost's libraries, we're using only boost's headers
# list(APPEND project_LIBS ${Boost_LIBRARIES})

add_subdirectory(src/thirdparty)

# list(APPEND REMATCH_LIBS spdlog)

add_subdirectory(src/rematch)

message("REmatch libs: ${REMATCH_LIBS}")

add_subdirectory(src/targets)

if (REMATCH_BUILD_TESTING)
  include(CTest)
  add_subdirectory(tests)
endif (REMATCH_BUILD_TESTING)
