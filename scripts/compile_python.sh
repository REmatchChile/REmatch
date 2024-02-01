#!/bin/bash
cd "$(dirname "$0")/.."

cmake_options=(
    -DCMAKE_C_COMPILER=clang
    -DCMAKE_CXX_COMPILER=clang++
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    -DBUILD_PYTHON_MODULE=ON
)

cmake "${cmake_options[@]}" -H. -Bbuild/Python
cmake --build build/Python
