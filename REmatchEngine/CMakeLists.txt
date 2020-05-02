cmake_minimum_required(VERSION 3.10)

enable_testing()

project(rematch LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set (MAINFOLDER ${PROJECT_SOURCE_DIR})

list (APPEND CMAKE_MODULE_PATH "${MAINFOLDER}/cmake")
set (EXECUTABLE_OUTPUT_PATH "${MAINFOLDER}/bin")

# add_definitions(-DNOPT_CROSSPROD)

include_directories (src)
include_directories (SYSTEM include)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	FIND_PACKAGE (Boost 1.58)
	INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})
	LIST (APPEND project_LIBS ${Boost_LIBRARIES})
endif()

add_subdirectory(src)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	add_subdirectory(tests)
endif()

# Check if compiling with emscripten
if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	set_target_properties(spanners_interface PROPERTIES LINK_FLAGS " --bind -s ERROR_ON_UNDEFINED_SYMBOLS=0")
endif()