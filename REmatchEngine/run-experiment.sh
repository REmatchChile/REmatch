#!/bin/bash

# BIN_FILE='./run-average.sh'
BIN_FILE='bin/rematch'

DATASET_DIRECTORY="datasets/RKBExplorer/"

EXP_DIRECTORY="exp/datasets/RKBExplorer/"



f="${EXP_DIRECTORY}/exp4"
REGEX="${f}/regex.rgx"


printf "Experiment with regex: $(sed -e 's/\\/\\\\/g' $REGEX)\n\"$(cat $f/description.txt)\"\n\n"


RESULT="${f}/results.txt"

echo -n "" > $RESULT


for i in {2..11}
do
	FILE="sparql.log.combo.$i"
	FILENAME="${DATASET_DIRECTORY}${FILE}"
	FILESIZE=$(du -h "$FILENAME" | awk '{print $1}')
	FILELINES=$(wc -l "$FILENAME" | awk '{print $1}')

	printf "Filename: ${FILE}\nFilesize: ${FILESIZE}\nFilelines: ${FILELINES}\nTime: " | tee -a $RESULT

	(timeout 600 $BIN_FILE -d $FILENAME -r $REGEX $1 -o benchmark | grep "Total time" | awk '{print $3}' \
								| sed -e 's/s//g' || echo -e "Killed") | tee -a $RESULT
	printf "\n" | tee -a $RESULT
done

printf "\n-----------------------------------\n\n" | tee -a $RESULT

