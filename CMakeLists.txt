# Based on https://github.com/robotology/how-to-export-cpp-library/

cmake_minimum_required(VERSION 3.23.2)

# Set project's name and version
project(REmatch
        LANGUAGES CXX C
        VERSION 1.0.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)

# Use all cores but one for compilation
include(ProcessorCount)
ProcessorCount(NUM_CORES)
if(NUM_CORES GREATER 1)
    math(EXPR PARALLEL_LEVEL "${NUM_CORES} - 1")
    set(CMAKE_BUILD_PARALLEL_LEVEL ${PARALLEL_LEVEL})
endif()

# Enable exporting compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Defines the CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_BINDIR and many other useful macros.
# See https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
include(GNUInstallDirs)

# Include CMake print helpers
include(CMakePrintHelpers)

# Control where libraries and executables are placed during the build.
# With the following settings executables are placed in <the top level of the
# build tree>/bin and libraries/archives in <top level of the build tree>/lib.
# This is particularly useful to run ctests on libraries built on Windows
# machines: tests, which are executables, are placed in the same folders of
# dlls, which are treated as executables as well, so that they can properly
# find the libraries to run. This is a because of missing RPATH on Windows.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
# See https://cmake.org/cmake/help/v3.4/variable/CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS.html
# See https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Under MSVC, we set CMAKE_DEBUG_POSTFIX to "d" to add a trailing "d" to library
# built in debug mode. In this Windows user can compile, build and install the
# library in both Release and Debug configuration avoiding naming clashes in the
# installation directories.
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Build position independent code.
# Position Independent Code (PIC) is commonly used for shared libraries so that
# the same shared library code can be loaded in each program address space in a
# location where it will not overlap with any other uses of such memory.
# In particular, this option avoids problems occurring when a process wants to
# load more than one shared library at the same virtual address.
# Since shared libraries cannot predict where other shared libraries could be
# loaded, this is an unavoidable problem with the traditional shared library
# concept.
# Generating position-independent code is often the default behavior for most
# modern compilers.
# Moreover linking a static library that is not built with PIC from a shared
# library will fail on some compiler/architecture combinations.
# Further details on PIC can be found here:
# https://eli.thegreenplace.net/2011/11/03/position-independent-code-pic-in-shared-libraries/
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

### Options
# Shared/Dynamic or Static library?
option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" OFF)

# Build test related commands?
option(BUILD_TESTING "Create tests using CMake" OFF)
if(BUILD_TESTING)
    enable_testing()
endif()

# Enable profiling?
option(ENABLE_PROFILING "Enable profiling" OFF)

# Enable RPATH support for installed binaries and libraries
include(AddInstallRPATHSupport)
add_install_rpath_support(BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"
                          LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
                          USE_LINK_PATH)

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

# ANTLR
add_subdirectory(third_party/antlr4-cpp-runtime)
include_directories(third_party/antlr4-cpp-runtime/src)

file(GLOB_RECURSE COMMON_SRC src/rematch/*.cpp)
file(GLOB_RECURSE COMMON_HDR src/rematch/*.hpp)
include_directories(src/rematch)

### Compile- and install-related commands.
add_subdirectory(src)

# Create and install CMake configuration files for your project that are
# necessary to for other projects to call find_package(LibTemplateCMake).
#
# Note that it is extremely important to use exactly the project name while
# installing configuration files (you can use PROJECT_NAME variable to avoid
# any possible error). This is required to allow find_package() to properly
# look for the installed library in system path, in particular in Windows when
# the installation is performed in the default path.
#
# install_basic_package_files() comes with many input parameters to customize
# the configuration files. The parameters used in the following call provide
# basic versions of CMake configuration files.
# See install_basic_package_files() documentation found in ./cmake folder.
#
# Note that if your library depends from other libraries, you are probably
# required to used the install_basic_package_files() DEPENDENCIES option.
include(InstallBasicPackageFiles)
install_basic_package_files(${PROJECT_NAME}
                            VERSION ${${PROJECT_NAME}_VERSION}
                            COMPATIBILITY AnyNewerVersion
                            VARS_PREFIX ${PROJECT_NAME}
                            NO_CHECK_REQUIRED_COMPONENTS_MACRO)
# Add the uninstall target
include(AddUninstallTarget)

# Add integration tests (unit tests for each library should be in each sublibrary directory).
cmake_print_variables(BUILD_TESTING)
if(BUILD_TESTING AND NOT SKBUILD AND NOT EMSCRIPTEN)
    add_subdirectory(test)
endif()

# Enable profiling
cmake_print_variables(ENABLE_PROFILING)
if(ENABLE_PROFILING AND NOT SKBUILD AND NOT EMSCRIPTEN)
  include(cmake/setup_tracy.cmake)
  set(TRACY_ENABLE ON CACHE BOOL "Enable Tracy" FORCE)
  set(TRACY_NO_EXIT ON CACHE BOOL "No Exit Tracy" FORCE)
  set(TRACY_DEBUGINFOD ON CACHE BOOL "Use DEBUGINFOD Tracy" FORCE)
  target_link_libraries(REmatch PUBLIC TracyClient)
endif()