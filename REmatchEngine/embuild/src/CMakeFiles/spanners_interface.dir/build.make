# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/nicovsj/repos/SpannersConst

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nicovsj/repos/SpannersConst/embuild

# Include any dependencies generated for this target.
include src/CMakeFiles/spanners_interface.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/spanners_interface.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/spanners_interface.dir/flags.make

src/CMakeFiles/spanners_interface.dir/wasminterface.cpp.o: src/CMakeFiles/spanners_interface.dir/flags.make
src/CMakeFiles/spanners_interface.dir/wasminterface.cpp.o: src/CMakeFiles/spanners_interface.dir/includes_CXX.rsp
src/CMakeFiles/spanners_interface.dir/wasminterface.cpp.o: ../src/wasminterface.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nicovsj/repos/SpannersConst/embuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/spanners_interface.dir/wasminterface.cpp.o"
	cd /home/nicovsj/repos/SpannersConst/embuild/src && /home/nicovsj/.emsdk/upstream/emscripten/em++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/spanners_interface.dir/wasminterface.cpp.o -c /home/nicovsj/repos/SpannersConst/src/wasminterface.cpp

src/CMakeFiles/spanners_interface.dir/wasminterface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/spanners_interface.dir/wasminterface.cpp.i"
	cd /home/nicovsj/repos/SpannersConst/embuild/src && /home/nicovsj/.emsdk/upstream/emscripten/em++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nicovsj/repos/SpannersConst/src/wasminterface.cpp > CMakeFiles/spanners_interface.dir/wasminterface.cpp.i

src/CMakeFiles/spanners_interface.dir/wasminterface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/spanners_interface.dir/wasminterface.cpp.s"
	cd /home/nicovsj/repos/SpannersConst/embuild/src && /home/nicovsj/.emsdk/upstream/emscripten/em++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nicovsj/repos/SpannersConst/src/wasminterface.cpp -o CMakeFiles/spanners_interface.dir/wasminterface.cpp.s

# Object files for target spanners_interface
spanners_interface_OBJECTS = \
"CMakeFiles/spanners_interface.dir/wasminterface.cpp.o"

# External object files for target spanners_interface
spanners_interface_EXTERNAL_OBJECTS =

../bin/spanners_interface.js: src/CMakeFiles/spanners_interface.dir/wasminterface.cpp.o
../bin/spanners_interface.js: src/CMakeFiles/spanners_interface.dir/build.make
../bin/spanners_interface.js: src/librematchlib.a
../bin/spanners_interface.js: src/CMakeFiles/spanners_interface.dir/linklibs.rsp
../bin/spanners_interface.js: src/CMakeFiles/spanners_interface.dir/objects1.rsp
../bin/spanners_interface.js: src/CMakeFiles/spanners_interface.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/nicovsj/repos/SpannersConst/embuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/spanners_interface.js"
	cd /home/nicovsj/repos/SpannersConst/embuild/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/spanners_interface.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/spanners_interface.dir/build: ../bin/spanners_interface.js

.PHONY : src/CMakeFiles/spanners_interface.dir/build

src/CMakeFiles/spanners_interface.dir/clean:
	cd /home/nicovsj/repos/SpannersConst/embuild/src && $(CMAKE_COMMAND) -P CMakeFiles/spanners_interface.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/spanners_interface.dir/clean

src/CMakeFiles/spanners_interface.dir/depend:
	cd /home/nicovsj/repos/SpannersConst/embuild && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nicovsj/repos/SpannersConst /home/nicovsj/repos/SpannersConst/src /home/nicovsj/repos/SpannersConst/embuild /home/nicovsj/repos/SpannersConst/embuild/src /home/nicovsj/repos/SpannersConst/embuild/src/CMakeFiles/spanners_interface.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/spanners_interface.dir/depend

