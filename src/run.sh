# !/bin/bash
make clean
make
./SGenerator -i log.csv -p inactivity_period.txt
