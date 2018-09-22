//Include common.h -> all libraries, structs, and queue.c prototypes in here
#include "common.h"

//Prototypes
void setup_shared_mem(int,printer*);
void init_shared_mem(int);

void take_a_job(job*,printer*);
void print_msg(job*,printer*);
void go_sleep(job*,printer*);
void init_printer(shared_data*, printer*);

void server_close(int); //server_error will be used with signal(); this will detach the shared mem from program


//Global variables
int fd;
shared_data* data;

int main(int argc, char* argv[]){
    //For printing, we want to write to stdout as soon as printf is written -> _IONBF is unbuffered so
    //text appears to standard out as soon as written
    setvbuf(stdout, NULL, _IONBF, 0);
    //Check the command-line arguments; exit the program if <number_of_slots> not specified
    if(argc < 2){
        printf("Invalid number of arguments; specify: ./server <number_of_slots>\n");
        printf("NOTE: <number_of_slots> is only significant for the first instance of the printer-server\n");
        exit(EXIT_FAILURE);
    }
    int number_of_slots = atoi(argv[1]);
    if(number_of_slots > BUFFER_CAPACITY){
        printf("Invalid arg: <number_of_slots> -> number larger than BUFFER_CAPACITY\n");
        exit(EXIT_FAILURE);
    }
    printer server;
    //signal for ctrl-c
    signal(SIGINT, server_close);

    setup_shared_mem(number_of_slots,&server);

    while(1){
        job printer_job;

        take_a_job(&printer_job,&server);
        print_msg(&printer_job,&server);
        go_sleep(&printer_job, &server);

    }
    server_close(0);
    return 0;
}

//Signal interrupt occurred -> detach from shared memory, close(fd), and exit
void server_close(int sigID){
    printf("server_close ran due to SIGINT or end of while loop\n");
    if(data != NULL){
        quit_printer(data); //decrement printer count; if we have no active printers, unlink; else, just munmap
        if(data->activePrinters <= 0) {
            printf("There are no longer any active printers, so the process will be unlinked from shared memory\n");
            close(fd);
            shm_unlink(MY_SHM);
            exit(0);
        }
        else{
            if(munmap(data, sizeof(shared_data)) == -1) perror("munmap failed in printer's server_close");
            close(fd);
            exit(0);
        }
    }
}

void setup_shared_mem(int number_of_slots, printer* server){
    //open using O_CREAT | O_EXCL -> if the fd == -1, then this shared memory is already open
    //so don't re-initialized everything
    fd = shm_open(MY_SHM, O_CREAT | O_EXCL, 0666);
    if(fd != -1){
        //We just created the shared memory; re-open with O_RDWR to read or write to it
        fd = shm_open(MY_SHM, O_RDWR, 0666);
        //Use truncate to make shared memory section of the appropriate length
        ftruncate(fd, sizeof(shared_data));
        //map to shared memory using fd, and cast to shared_data*
        data = (shared_data*) mmap(NULL, sizeof(shared_data),PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(data == MAP_FAILED){
            perror("Failed to mmap() to shared memory\n");
            exit(EXIT_FAILURE);
        }
        init_shared_mem(number_of_slots); //Since we just created the shared mem, we must initialize the variables
        init_printer(data, server); //Increment the number of active printers
        return;
    }
    //Else, fd == -1 -> the shared memory has been set up; so we can re-open the shared memory
    //and map to it, but we don't need to initialize the shared memory
    fd = shm_open(MY_SHM, O_RDWR, 0666);

    ftruncate(fd, sizeof(shared_data));
    data = (shared_data*) mmap(NULL, sizeof(shared_data),PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(data == MAP_FAILED){
        perror("Failed to mmap() to shared memory");
        exit(EXIT_FAILURE);
    }
    init_printer(data,server);
    return;
}
//Initialize shared memory data in the shared_data* struct
void init_shared_mem(int number_of_slots){
    //Initialize integers
    data->activeJobs = 0;
    data->number_of_slots = number_of_slots;
    data->printerIDCounter = 0;
    data->activePrinters = 0;
    //Initialize semaphores
    sem_init(&(data->mutex), 1, 1); //mutual exclusion to access queue
    sem_init(&(data->overflow), 1, data->number_of_slots); //overflow semaphore
    sem_init(&(data->underflow), 1, 0); //underflow semaphore
    sem_init(&(data->printerMUTEX), 1, 1); //mutual exclusion when incrementing/decrementing active printer count
}

void take_a_job(job* printer_job,printer* server){
    printf("Printer %d entering shared memory\n", server->id);
    dequeue(data, printer_job); //dequeue a job from shared memory queue
    return;
}

void print_msg(job* printer_job,printer* server){
    //Print message using job received from shared memory
    printf("Printer %d starts printing %d pages from Client %d\n",server->id, printer_job->page_count,printer_job->job_source.id);
}

void go_sleep(job* printer_job, printer* server){
    //Sleep according to job duration
    sleep(printer_job->duration);
    printf("Printer %d finishes printing %d pages from Client %d\n",server->id,printer_job->page_count, printer_job->job_source.id);
}