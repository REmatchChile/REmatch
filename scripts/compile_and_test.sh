#/usr/bin
cmake -B build/Debug -D CMAKE_BUILD_TYPE=Debug && cmake --build build/Debug
retVal=$?
if [ $retVal -ne 0 ]; then
    exit $retVal
fi
build/Debug/bin/rematch-test-d
