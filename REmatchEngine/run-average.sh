#!/bin/bash

BIN_FILE="bin/rematch"

N_EXPERMIENTS=5

sum=0

for run in {1..5}
do
				time=$($BIN_FILE -d $1 -r $2 $3 -o benchmark \
								| grep "Total time" | awk '{print $3}' \
								| sed -e 's/s//g')

				sum=$(echo "$sum + $time" | bc)
done

echo "$(echo "scale=4; $sum / 5" | bc -l)s"
