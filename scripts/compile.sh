#!/bin/bash
# Change to parent directory
cd "$(dirname "$0")/.."

cmake -H. -Bbuild/Release
cmake --build build/Release
