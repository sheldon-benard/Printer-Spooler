#include "common.h"

//Prototypes
void setup_client_memory();
void setup_client(client*,int);
void create_job(job*,client*);
void put_job(job*,client*);
void release_shared_mem();

//globals
int fd;
shared_data* data;

int main(int argc, char* argv[]){
    //For printing, we want to write to stdout as soon as printf is written -> _IONBF is unbuffered so
    //text appears to standard out as soon as written
    setvbuf(stdout, NULL, _IONBF, 0);
    int clientID; //clientID == source number specified as command-line argument
    if(argc < 2){
        printf("Invalid number of arguments; specify: ./client <source_number>\n");
        exit(EXIT_FAILURE);
    }
    clientID = atoi(argv[1]);
    client c;
    job curr_job;

    setup_client_memory();
    int bool = uniqueClientID(data, clientID); //check in shared memory that the clientID is unique
    //bool == -1 -> clientID is not unique
    if(bool == -1){
        //if bool == -1, not unique; munmap from shared_mem and exit
        printf("Invalid source_number; source_number must be unique\n");
        release_shared_mem();
        exit(EXIT_FAILURE);
    }
    //Thus, clientID is unique
    setup_client(&c, clientID);
    create_job(&curr_job, &c);
    put_job(&curr_job, &c);
    release_shared_mem();
    return 0;

}

void setup_client_memory(){
    //shm_open using MY_SHM ID in common.h
    fd = shm_open(MY_SHM, O_RDWR,0666);
    if(fd == -1){
        perror("shm_open failed in setup_client_memory\n");
        exit(EXIT_FAILURE);
    }
    //mmap into shared memory
    data = (shared_data*) mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(data == MAP_FAILED){
        perror("mmap failed in setup_client_memory\n");
        exit(EXIT_FAILURE);
    }
    return;

}

void release_shared_mem(){
    //we want to un-map the client
    if(munmap(data, sizeof(shared_data)) == -1) perror("munmap failed in client's release_shared_mem");
    close(fd);
    exit(0);
}

void setup_client(client* c, int clientID){
    //clientID is unique, so we can finish initializing client c such that c.id = clientID
    c->id = clientID;
    return;
}

void create_job(job* curr_job, client* c){
    //Create job by specifying job's source, duration and page_count
    curr_job->job_source = *c;
    //duration and pageCount will be randomly generated between 1-10
    srand((unsigned)time(0)*getpid());
    int duration = (rand() % 10) + 1; //between 1 and 10
    int pageCount = (rand() % 10) + 1; //between 1 and 10
    curr_job->duration = duration;
    curr_job->page_count = pageCount;
}

void put_job(job* curr_job, client* c){
    //enqueue job in shared memory queue
    enqueue(data, curr_job);
    printf("Client %d has %d pages to print, puts request in Buffer\n",c->id,curr_job->page_count);
    return;
}