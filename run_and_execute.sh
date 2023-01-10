#!/bin/bash
cmake -D CMAKE_C_COMPILER=gcc-11 -D CMAKE_CXX_COMPILER=g++-11 -H. -Bbuild/Release
cmake --build build/Release
build/Release/bin/rematch_test
