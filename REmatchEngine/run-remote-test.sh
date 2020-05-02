#!/bin/bash

SOURCE_PATH="./exp/experiment.cpp"
EXPERIMENTS_SCRIPT="run-experiment.sh"

# Compile C++ program
echo "Compiling ${SOURCE_PATH}"

g++ -O3 $SOURCE_PATH -o exp.out -std=gnu++0x && echo "Compiled succesfully."

# Copy binary and shell script to server
echo "Uploading binary and shell script to server..."
scp exp.out ciws:bin/exp.out
scp $EXPERIMENTS_SCRIPT ciws:bin/exp.sh

ssh -t ciws 'rm -rf exp'

scp -r exp/datasets ciws:exp/

rm -f exp.out

# Run experiments in server
echo "Running experiments..."
ssh -t ciws 'bin/exp.sh bin/exp.out'

# Retrieve the results 
echo "Retriving results..."
scp -r ciws:exp/* exp/datasets/


for f in exp/datasets/*/* 
do
	csv-to-json "${f}/results.csv" "${f}/results.json"
done

