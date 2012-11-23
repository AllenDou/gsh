#!/bin/sh
./bin/bc_cli -h 127.0.0.1 -p 6522 -e "2*((1+2+3+5)+3)"
./bin/carsvm -h 127.0.0.1 -p 6522 
#./bin/fmload -h 127.0.0.1 -p 6522 -f sina
