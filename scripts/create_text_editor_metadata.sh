#!/bin/bash
./compile.sh
ln -s -r ../build/Release/compile_commands.json ../compile_commands.json
ctags -R ../src ../tests
mv tags ../
