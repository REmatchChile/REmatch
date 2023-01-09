#!/bin/bash
cmake -H. -Bbuild/Release
cmake --build build/Release
build/Release/bin/rematch test.txt -e '!x{b}'
