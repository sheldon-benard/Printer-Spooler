#!/bin/bash

#Run same clientID 5 times
#3 jobs will go immediately into the waiting printers
#1 job will wait in the queue (it will be the only job in the queue)
#1 job will be requested, since client 5 already has something in the queue

./client 5 >> non_unique_log.txt &
./client 5 >> non_unique_log.txt &
./client 5 >> non_unique_log.txt &
./client 5 >> non_unique_log.txt &
./client 5 >> non_unique_log.txt &

