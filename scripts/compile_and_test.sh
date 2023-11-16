#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
PURPLE='\033[0;35m'
NORMAL_OUTPUT='\033[0m'

# Change to parent directory
cd "$(dirname "$0")/.."

# Compile the project under "Debug"
./scripts/compile.sh
build_result=$?

if [ $build_result -ne 0 ]; then
    echo -e "${RED}Build failed!${NORMAL_OUTPUT}"
    exit 1
fi

build/Release/bin/rematch_test
