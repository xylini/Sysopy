//
// Created by Jakub Pajor on 12.05.2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <zconf.h>

#include "common.h"

int shared_memory_id;
int semaphore_id;

void handle_signal(int _) {
    printf("Closing barbershop\n");
    exit(0);
}

void invite_client() {
    pid_t client_pid = barbershop->queue[0];
    barbershop->selected_client = client_pid;
    printf(BLUE_COLOR "# %lo #" RESET_COLOR "barber: invited client %i\n", get_timestamp(), client_pid);
}

void serve_client() {
    printf(BLUE_COLOR "# %lo #" RESET_COLOR "barber: started shaving client %i\n",
           get_timestamp(),
           barbershop->selected_client);

    printf(BLUE_COLOR "# %lo #" RESET_COLOR "barber: finished shaving client %i\n",
           get_timestamp(),
           barbershop->selected_client);

    barbershop->selected_client = 0;
}

void clean_up() {
    // Remove shared memory
    if(semaphore_id != 0) {
        semctl(semaphore_id, 0, IPC_RMID);
    }

    // Remove semaphore
    if(shared_memory_id != 0) {
        shmctl(shared_memory_id, IPC_RMID, NULL);
    }
}

void init(int argc, char** argv) {
    // Handle signals and atexit() callback
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);
    atexit(clean_up);

    // Control count of arguments
    if (argc < 2) PRINT_AND_EXIT("Not enough arguments provided\n")

    // Handle room size argument
    int waiting_room_size = (int) strtol(argv[1], 0, 10);
    if (waiting_room_size > MAX_QUEUE_SIZE)
        PRINT_AND_EXIT("Provided room size was too big\n")

    // Obtain project key
    key_t project_key = ftok(PROJECT_PATH, PROJECT_ID);
    if (project_key == -1)
        PRINT_AND_EXIT("Couldn't obtain a project key\n")

    // Create shared memory
    shared_memory_id = shmget(
            project_key,
            sizeof(struct Barbershop),
            S_IRWXU | IPC_CREAT
    );

    if (shared_memory_id == -1)
        PRINT_AND_EXIT("Couldn't create shared memory\n")

    // Access shared memory
    barbershop = shmat(shared_memory_id, 0, 0);
    if (barbershop == (void*) -1)
        PRINT_AND_EXIT("Couldn't access shared memory\n")

    // Create set with one semaphore
    semaphore_id = semget(project_key, 1, IPC_CREAT | S_IRWXU);

    if (semaphore_id == -1)
        PRINT_AND_EXIT("Couldn't create semaphore\n")

    semctl(semaphore_id, 0, SETVAL, 0);

    // Initialize the barbershop
    barbershop->barber_status = SLEEPING;
    barbershop->waiting_room_size = waiting_room_size;
    barbershop->client_count = 0;
    barbershop->selected_client = 0;

    // Initialize empty clients queue
    for (int i = 0; i < MAX_QUEUE_SIZE; ++i) barbershop->queue[i] = 0;
}

// MAIN ////////////////////////////////////////////////////////////////////////

// Takes one argument: waiting room size
int main(int argc, char** argv) {
    init(argc, argv);

    release_semaphore(semaphore_id);

    while(1) {
        get_semaphore(semaphore_id);

        switch (barbershop->barber_status) {
            case IDLE:
                if (is_queue_empty()) {
                    printf(BLUE_COLOR "# %lo #" RESET_COLOR "barber: barber fell asleep\n", get_timestamp());
                    barbershop->barber_status = SLEEPING;
                } else {
                    // Invite client from queue and then go to ready state
                    // (which will make the barber serve the customer)
                    invite_client();
                    barbershop->barber_status = READY;
                }
                break;
            case AWOKEN:
                printf(BLUE_COLOR "# %lo #" RESET_COLOR "barber: woke up\n", get_timestamp());
                barbershop->barber_status = READY;
                break;
            case BUSY:
                serve_client();
                barbershop->barber_status = READY;
                break;
            default:
                break;
        }

        release_semaphore(semaphore_id);
    }
}