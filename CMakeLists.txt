cmake_minimum_required(VERSION 3.14)

project(rematch LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
	message(STATUS "Build type not specified. Using Release by default.")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable testing
set(REMATCH_BUILD_TESTING ON)
set(REMATCH_BUILD_BENCHMARKS OFF)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pthread -O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -pthread -O3 -g0")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

add_compile_definitions(
	MACRO_TRANSITIONS_RAW_ARRAYS
	# COUNT_CURRENT_STATES
	# NOPT_CROSSPROD
	# NOPT_OFFSET
	# NOPT_MEMORY
)

# Rematch libs starts as empty list
set(REMATCH_LIBS "")

include_directories(src/rematch)
include_directories(SYSTEM src/thirdparty)

find_package(Boost 1.58)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
# No need to add Boost's libraries, we're using only boost's headers
# list(APPEND project_LIBS ${Boost_LIBRARIES})

add_subdirectory(src/rematch)

message("REmatch libs: ${REMATCH_LIBS}")

add_subdirectory(src/targets)

if(REMATCH_BUILD_TESTING)
	add_subdirectory(tests)
endif(REMATCH_BUILD_TESTING)


