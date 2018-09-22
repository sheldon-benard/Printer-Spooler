#include "common.h"
//We'll use queue.c to handle the producer-consumer critical section

//dequeue will deal with removing the front job from the queue and moving everything over in the queue
//it'll be reliant on the mutex semaphore and the underflow semaphore
void dequeue(shared_data* data, job* curr_job){
    //wait on the underflow condition -> if there are no jobs, can't remove a job & and mutex
    sem_wait(&(data->underflow));
    sem_wait(&(data->mutex));

    //Error condition: if the activeJobs < 1; error
    if(data->activeJobs < 1) exit(EXIT_FAILURE);

    //now, copy queue first job and copy it into the job pointer
    memcpy(curr_job, &(data->active_jobs_queue[0]), sizeof(job));

    //now shift all jobs down by one;
    int j;
    for(j=1; j< data->activeJobs; j++){
        memcpy(&(data->active_jobs_queue[j-1]),&(data->active_jobs_queue[j]), sizeof(job));
    }

    //decrement active jobs
    data->activeJobs = data->activeJobs - 1;

    //now, we are done with critical section -> we can post the mutex and the overflow
    sem_post(&(data->mutex));
    sem_post(&(data->overflow));

}

int uniqueClientID(shared_data* data, int ID){
    sem_wait(&(data->mutex)); //Although this function won't change the buffer, we don't want other processes to
    //change the buffer while we are checking for repeated clientID's
    int result = 1;
    int i;
    for(i=0;i<data->activeJobs;i++){
        if(data->active_jobs_queue[i].job_source.id == ID){
            result = -1;
            break;
        }
    }
    sem_post(&(data->mutex));
    return result;
}

//enqueue will deal with adding job to the end of the queue (since queue is FIFO); since we have a count
//of how many jobs we have (data->activeJobs), this will not require a loop;
// curr_job will be the job to enter the queue
void enqueue(shared_data* data, job* curr_job){
    //this is the client, so we will have to wait on overflow and mutex
    sem_wait(&(data->overflow));
    sem_wait(&(data->mutex));

    //Error condition: if the activeJobs >= BufferCapacity -> exit since we can't add another job
    if(data->activeJobs >= data->number_of_slots) exit(EXIT_FAILURE);

    memcpy(&(data->active_jobs_queue[data->activeJobs]), curr_job, sizeof(job));

    //increment activeJobs
    data->activeJobs = data->activeJobs + 1;

    //post mutex and underflow, since we just added a job to the queue
    sem_post(&(data->mutex));
    sem_post(&(data->underflow));

}
//Use printerMUTEX to establish mutual exclusivity when establishing printerID
void init_printer(shared_data* data, printer* server){
    sem_wait(&(data->printerMUTEX));
    server->id = data->printerIDCounter;
    data->printerIDCounter = data->printerIDCounter + 1; //We can make this change
    data->activePrinters = data->activePrinters + 1;
    sem_post(&(data->printerMUTEX));
}
//mutual excusivity when affecting activePrinter count
void quit_printer(shared_data* data){
    sem_wait(&(data->printerMUTEX));
    data->activePrinters = data->activePrinters - 1;
    sem_post(&(data->printerMUTEX));
}