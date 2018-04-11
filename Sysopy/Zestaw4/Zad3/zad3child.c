//
// Created by Jakub Pajor on 11.04.2018.
//
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <memory.h>

int SIGUSR1_received = 0;
int TYPE = 0;

#define hearing_and_mask(signal1, signal2){                 \
        struct sigaction action;                            \
        sigemptyset(&action.sa_mask);                       \
        action.sa_flags = SA_SIGINFO;                       \
        action.sa_sigaction = signals_handler;              \
        sigset_t mask;                                      \
        sigfillset(&mask);                                  \
                                                            \
        sigdelset(&mask, signal1);                          \
        sigdelset(&mask, signal2);                          \
                                                            \
        if (sigaction(signal1, &action, NULL) == -1)        \
            fprintf(stderr,"Can't catch %d.\n",signal1);     \
        if (sigaction(signal2, &action, NULL) == -1)        \
            fprintf(stderr,"Can't catch %d.\n",signal2);     \
        sigprocmask(SIG_SETMASK, &mask, NULL);              \
}

void signals_handler(int signum, siginfo_t *info, void *context) {
    if (info->si_pid != getppid()) return;

    if (signum == SIGUSR1 || signum == SIGRTMIN) {
        SIGUSR1_received++;
        kill(getppid(), SIGUSR1);
        fprintf(stderr,"Child: signal %d received.\n",signum);
	fprintf(stderr,"Child: Sending it back.\n");
    } else if (signum == SIGUSR2 || signum == SIGRTMAX) {
        SIGUSR1_received++;
        fprintf(stderr, "\nChild: signals received: %d.\n", SIGUSR1_received);
        fprintf(stderr, "Child: signal %d received. Suicide.\n", signum);
        exit((unsigned) SIGUSR1_received);
    }
}



int main(int argc, char * argv[]){
    TYPE = atoi(argv[1]);
    if (TYPE == 1 || TYPE == 2) hearing_and_mask(SIGUSR1, SIGUSR2);
    if (TYPE == 3)              hearing_and_mask(SIGRTMIN, SIGRTMAX);

    while (1) { sleep(1); }
}
