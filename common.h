#ifndef _INCLUDE_COMMON_H_
#define _INCLUDE_COMMON_H_

//For important functions
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// Required for the shared memory
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include <signal.h>
#include <semaphore.h>
//For random number generator:
#include <time.h>

#define BUFFER_CAPACITY 100 /* As specified on my courses, we can specify a static bound for the buffer */

#define MY_SHM "SHAREDMEM"

//Client stuct has ID
typedef struct{
    int id;
} client;
//Printer struct has ID
typedef struct{
    int id;
} printer;
//Job struct has page_count, duration of job, and the source of the job
typedef struct{
    int page_count;
    int duration;
    client job_source;
} job;
/* shared_data struct
 * mutex, overflow, and underflow semaphores for consumer-producer problem and check for unique client ID
 * printerMUTEX for printerID and activePrinter counter
 * */
typedef struct{
    sem_t mutex;
    sem_t overflow;
    sem_t underflow;
    sem_t printerMUTEX; //for the printerID
    int activeJobs; //Number of jobs in queue
    int activePrinters; //activePrinters in system
    int number_of_slots; //subset of BUFFER_CAPACITY to be used by printer-spooler system
    int printerIDCounter; //counter to establish printer-server ID
    job active_jobs_queue[BUFFER_CAPACITY]; //queue of active jobs
} shared_data;

//queue.c critical area functions
void enqueue(shared_data*, job* );
void dequeue(shared_data*, job* );
void init_printer(shared_data*, printer* );
int uniqueClientID(shared_data*, int);
void quit_printer(shared_data*);

#endif //_INCLUDE_COMMON_H_
