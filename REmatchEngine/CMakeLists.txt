cmake_minimum_required(VERSION 3.14)

enable_testing()

project(rematch LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
	message(STATUS "Build type not specified. Using Release by default.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3 -g0")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

if(SWIG)
	find_package(SWIG 4.0)
	find_package(Python3 REQUIRED COMPONENTS Development)
	include(${SWIG_USE_FILE})
	# Add support for .swg file
	set(SWIG_SOURCE_FILE_EXTENSIONS ".i" ".swg")
	set(CMAKE_SWIG_OUTDIR "${EXECUTABLE_OUTPUT_PATH}/SWIG")
endif()

# add_definitions(-DNOPT_CROSSPROD)
# add_definitions(-DNOPT_OFFSET)

include_directories(src)
include_directories(SYSTEM thirdparty)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	find_package(Boost 1.58)
	include_directories(${Boost_INCLUDE_DIRS})
	list(APPEND project_LIBS ${Boost_LIBRARIES})
endif()

add_subdirectory(src)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten" AND NOT SWIG)
	add_subdirectory(tests)
endif()