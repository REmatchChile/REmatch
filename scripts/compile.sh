#!/bin/bash
cd ..
cmake -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++ -H. -Bbuild/Release
cmake --build build/Release
