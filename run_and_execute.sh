#!/bin/bash
cmake -H. -Bbuild/Release
cmake --build build/Release
build/Release/bin/rematch_test
