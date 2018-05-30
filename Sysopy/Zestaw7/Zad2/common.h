//
// Created by Jakub Pajor on 12.05.2018.
//

#ifndef __COMMON_H__
#define __COMMON_H__

#include <errno.h>
#include <string.h>

// PRINTING --------------------------------------------------------------------

#define RED_COLOR "\e[1;31m"
#define RESET_COLOR "\e[0m"
#define BLUE_COLOR "\e[1;34m"

#define PRINT_AND_EXIT(msg, ...) {                                            \
    printf(RED_COLOR msg RESET_COLOR, ##__VA_ARGS__);                       \
    exit(-1);                                                                  }

// PROJECT CONSTS --------------------------------------------------------------------

#define PROJECT_PATH getenv("HOME")

// NOTE: actual size of the queue will be determined by the waiting_room_size
#define MAX_QUEUE_SIZE 32

// DATA --------------------------------------------------------------------

enum Barber_status {
    SLEEPING,
    AWOKEN,
    READY,
    IDLE,
    BUSY
};

enum Client_status {
    NEW,
    INVITED,
    SHAVED
};

struct Barbershop {
    enum Barber_status barber_status;
    int client_count;
    int waiting_room_size;
    pid_t selected_client;
    pid_t queue[MAX_QUEUE_SIZE];
} *barbershop;

// TIMESTAMPS --------------------------------------------------------------------

long get_timestamp() {
    struct timespec buffer;
    clock_gettime(CLOCK_MONOTONIC, &buffer);
    return buffer.tv_nsec / 1000;
}

// SEMAPHORE HANDLERS --------------------------------------------------------------------

void get_semaphore(sem_t* semaphore) {
    int error = sem_wait(semaphore);
    if (error == -1) PRINT_AND_EXIT("Semaphore error: %s", strerror(errno))
}

void release_semaphore(sem_t* semaphore) {
    int error = sem_post(semaphore);
    if (error == -1) PRINT_AND_EXIT("Semaphore error: %s", strerror(errno))
}

// QUEUE HANDLERS --------------------------------------------------------------------

int is_queue_full() {
    return barbershop->client_count < barbershop->waiting_room_size ? 0 : 1;
}

int is_queue_empty() {
    return barbershop->client_count == 0 ? 1 : 0;
}

void enter_queue(pid_t pid) {
    barbershop->queue[barbershop->client_count++] = pid;
}

void pop_queue() {
    int i = 0;
    // Shift queue content left
    while(i < barbershop->client_count - 1) {
        barbershop->queue[i] = barbershop->queue[i + 1];
        i++;
    }
    // After shift last pid is copied, change to 0
    barbershop->queue[barbershop->client_count - 1] = 0;
    barbershop->client_count--;
}
#endif
