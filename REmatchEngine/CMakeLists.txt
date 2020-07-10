cmake_minimum_required(VERSION 3.14)

enable_testing()

project(rematch LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
	message(STATUS "Build type not specified. Using Release by default.")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

if(SWIG)
	find_package(SWIG 4.0)
	find_package(Python3 REQUIRED COMPONENTS Development)
	include(${SWIG_USE_FILE})
	# Add support for .swg file
	set(SWIG_SOURCE_FILE_EXTENSIONS ".i" ".swg")
	set(CMAKE_SWIG_OUTDIR "${EXECUTABLE_OUTPUT_PATH}/SWIG")
endif()

# add_definitions(-DNOPT_CROSSPROD)

include_directories(src)
include_directories(SYSTEM include)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	find_package(Boost 1.58)
	include_directories(${Boost_INCLUDE_DIRS})
	list(APPEND project_LIBS ${Boost_LIBRARIES})
endif()

add_subdirectory(src)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten" AND NOT SWIG)
	add_subdirectory(tests)
endif()