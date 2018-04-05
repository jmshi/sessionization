#!/bin/bash
#
# Use this shell script to compile (if necessary) your code and then execute it. Below is an example of what might be found in this file if your program was written in Python
#
make clean
make
./bin/./SGenerator -i ./input/log.csv . -p ./input/inactivity_period.txt -o ./output/sessionization.txt
#python ./src/sessionization.py ./input/log.csv ./input/inactivity_period.txt ./output/sessionization.txt
