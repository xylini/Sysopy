//
// Created by Jakub Pajor on 12.05.2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <zconf.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#include <semaphore.h>

#include "common21.h"

// GLOBALS ----------------------------------------------------------------------

enum Client_status status;
int shared_memory_id;
sem_t* semaphore;

// UTILITIES ----------------------------------------------------------------------

void init() {

    shared_memory_id = shm_open(PROJECT_PATH, O_RDWR, S_IRWXU | S_IRWXG);
    if (shared_memory_id == -1) PRINT_AND_EXIT("Couldn't create shared memory\n")

    int error;
    error = ftruncate(shared_memory_id, sizeof(struct Barbershop));

    if (error == -1) PRINT_AND_EXIT("ftruncate failed.\n");

    barbershop = mmap(NULL,
                      sizeof(*barbershop),
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      shared_memory_id,
                      0);

    if (barbershop == (void*) -1)
        PRINT_AND_EXIT("Couldn't access shared memory\n");

    semaphore = sem_open(PROJECT_PATH, O_WRONLY, S_IRWXU | S_IRWXG, 0);

    if (semaphore == (void*) -1) PRINT_AND_EXIT("Couldn't create semaphore\n");
}

void claim_chair() {
    pid_t pid = getpid();

    if (status == INVITED) {
        pop_queue();
    } else if (status == NEW) {
        while (1) {
            release_semaphore(semaphore);
            get_semaphore(semaphore);
            if (barbershop->barber_status == READY) break;
        }
        status = INVITED;
    }
    barbershop->selected_client = pid;
    printf(BLUE_COLOR "# %lo #" RESET_COLOR "%i: took place in the chair\n", get_timestamp(), pid);
}

void run_client(int S) {
    pid_t pid = getpid();
    int cuts = 0;

    while (cuts < S) {
        // Handle client
        status = NEW;

        get_semaphore(semaphore);

        if (barbershop->barber_status == SLEEPING) {
            printf(BLUE_COLOR "# %lo #" RESET_COLOR "%i: woke up the barber\n", get_timestamp(), pid);
            barbershop->barber_status = AWOKEN;
            printf(BLUE_COLOR "# %lo #" RESET_COLOR "%i: entering the queue\n", get_timestamp(), pid);
            claim_chair();
            barbershop->barber_status = BUSY;
        } else if (!is_queue_full()) {
            enter_queue(pid);
            printf(BLUE_COLOR "# %lo #" RESET_COLOR "%i: entering the queue\n", get_timestamp(), pid);
        } else {
            printf(BLUE_COLOR "# %lo #" RESET_COLOR "%i: could not find place in the queue\n", get_timestamp(), pid);
            release_semaphore(semaphore);
            return;
        }

        release_semaphore(semaphore);

        while(status < INVITED) {
            get_semaphore(semaphore);
            if (barbershop->selected_client == pid) {
                status = INVITED;
                claim_chair();
                barbershop->barber_status = BUSY;
            }
            release_semaphore(semaphore);
        }

        while(status < SHAVED) {
            get_semaphore(semaphore);
            if (barbershop->selected_client != pid) {
                status = SHAVED;
                printf(BLUE_COLOR "# %lo #" RESET_COLOR "%i: shaved\n", get_timestamp(), pid);
                barbershop->barber_status = IDLE;
                cuts++;
            }
            release_semaphore(semaphore);
        }
    }
    printf(BLUE_COLOR "# %lo #" RESET_COLOR "%i: left barbershop after %i cuts\n", get_timestamp(), pid, S);
}

// MAIN ----------------------------------------------------------------------

// Takes two arguments:
// clients number - how many clients should be generated
// S              - number of cuts each client tries to get
int main(int argc, char** argv) {
    if (argc < 3) PRINT_AND_EXIT("./client [clients number] [shaves number]\n")
    int clients_number = (int) strtol(argv[1], 0, 10);
    int S = (int) strtol(argv[2], 0, 10);
    init();

    for (int i = 0; i < clients_number; ++i) {
        if (!fork()) {
            run_client(S);
            exit(0);
        }
    }
    while (wait(0)) if (errno != ECHILD) break;
}
