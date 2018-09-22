#!/bin/bash

#clean the printer and client executables
#make all
make clean
make all

#Set up three printers in background and append to log.txt
#Since printer 10 runs first, the buffer will have capacity of 10
./printer 10 >> log.txt &
./printer 12 >> log.txt &
./printer 13 >> log.txt &