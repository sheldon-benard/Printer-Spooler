#!/bin/bash

#run 15 clients, to enforce an overload of the queue

./client 1 >> log.txt &
./client 2 >> log.txt &
./client 3 >> log.txt &
./client 4 >> log.txt &
./client 5 >> log.txt &
./client 6 >> log.txt &
./client 7 >> log.txt &
./client 8 >> log.txt &
./client 9 >> log.txt &
./client 10 >> log.txt &
./client 11 >> log.txt &
./client 12 >> log.txt &
./client 13 >> log.txt &
./client 14 >> log.txt &
./client 15 >> log.txt &