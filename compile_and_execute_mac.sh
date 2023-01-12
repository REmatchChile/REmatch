#!/bin/bash
cmake -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ -H. -Bbuild/Release
cmake --build build/Release
build/Release/bin/rematch_test
