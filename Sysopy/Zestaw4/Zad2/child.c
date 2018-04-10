//
// Created by Jakub Pajor on 08.04.2018.
//
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

int freeze = 1;

void return_signal_handler(int signum){ freeze = 0; }

int main(int argc, char * argv[]){
    srand((unsigned int) (getpid()*13)%10);
    int random_time = rand() % 11;
    sleep(random_time);
    kill(getppid(),SIGUSR1);

    signal(SIGUSR2,return_signal_handler);
    while(freeze);
    int random_signal = rand() % (SIGRTMAX - SIGRTMIN + 1) + SIGRTMIN;
    kill(getppid(),random_signal);

    return random_time;
}
