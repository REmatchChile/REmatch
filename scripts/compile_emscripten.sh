#!/bin/bash
# Change to parent directory
cd "$(dirname "$0")/.."

emcmake cmake -H. -Bbuild/WASM
cmake --build build/WASM
