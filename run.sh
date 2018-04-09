#!/bin/bash
#  example calling sequence
#      ./bin/./SGenerator -i ./input/your_input_file . -p ./input/your_param_file -o ./output/your_output_file
#

# uncomment the following if you wish to compile the code here.
make clean
make
./bin/./SGenerator -i ./input/log.csv . -p ./input/inactivity_period.txt -o ./output/sessionization.txt

# An example:  2.6G web log from SEC's EDGAR
#./bin/./SGenerator -i ./input/log20170630.csv . -p ./input/inactivity_5min.txt -o ./output/session_log20170630.txt
