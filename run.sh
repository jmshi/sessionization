#!/bin/bash
#
# Use this shell script to compile (if necessary) your code and then execute it. Below is an example of what might be found in this file if your program was written in Python
#
#python ./src/sessionization.py ./input/log.csv ./input/inactivity_period.txt ./output/sessionization.txt
#
make clean
make
#./bin/./SGenerator -i ./input/log.csv . -p ./input/inactivity_period.txt -o ./output/sessionization.txt
./bin/./SGenerator -i ./input/log20170630.csv . -p ./input/inactivity_5min.txt -o ./output/session_log20170630.txt
