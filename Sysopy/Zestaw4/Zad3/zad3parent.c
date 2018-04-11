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

volatile int L = 0;
volatile int TYPE = 0;
volatile int sentToChild = 0;
volatile int receivedByChild = 0;
volatile int receivedFromChild = 0;
volatile pid_t child;



void signals_handler(int signum, siginfo_t *info, void *context) {
    if (signum == SIGINT) {
        printf("Parent: Received SIGINT. Killing child. Exit.\n");
        kill(child, SIGUSR2);
        exit(EXIT_FAILURE);
    }

    if ((signum == SIGUSR1 || signum == SIGRTMIN) && info->si_pid == child) {
        receivedFromChild++;
        printf("Parent: Received signal: %d form Child\n",signum);
    }
}

void parent_is_coming() {
    sleep(1);

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = signals_handler;

    if (sigaction(SIGINT, &act, NULL) == -1)
        fprintf(stderr,"Can't catch SIGINT\n");

    if (TYPE == 1 || TYPE == 2)
        if (sigaction(SIGUSR1, &act, NULL) == -1) fprintf(stderr,"Can't catch SIGUSR1\n");
    if (TYPE == 3)
        if (sigaction(SIGRTMIN, &act, NULL) == -1) fprintf(stderr,"Can't catch SIGRTMIN\n");

    if (TYPE == 1 || TYPE == 2) {
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGINT);
        while(sentToChild < L){
            printf("Parent: Sending SIGUSR1\n");
            kill(child, SIGUSR1);
            if (TYPE == 2) sigsuspend(&mask);
	    sentToChild++;
        }
        printf("Parent: Sending SIGUSR2\n");
        kill(child, SIGUSR2);
    }
    if (TYPE == 3) {
        while(sentToChild < L){
            printf("Parent: Sending SIGRTMIN\n");
            kill(child, SIGRTMIN);
	    sentToChild++;
        }
        sentToChild++;
        printf("Parent: Sending SIGRTMAX\n");
        kill(child, SIGRTMAX);
    }

    int status = 0;
    waitpid(child, &status, 0);
    if (WIFEXITED(status))
        receivedByChild = WEXITSTATUS(status);
    else fprintf(stderr,"Sth went wrong with termination of Child!\n");
}


int main(int argc, char *argv[]) {

    if (argc < 3) {fprintf(stderr,"Please give arguments: first - signals numbers, second - mode.\n"); exit(EXIT_FAILURE); }
    L = atoi(argv[1]);
    if(L < 1) {fprintf(stderr,"Wrrrr... Given sig numbers is wrong.\n"); exit(EXIT_FAILURE);}

    TYPE = atoi(argv[2]);
    if (TYPE < 1 || TYPE > 3) {fprintf(stderr,"Wrrrr... Mode argument should be 1, 2 or 3.\n"); exit(EXIT_FAILURE);}
    
    if ((child = fork()) == 0) {execl("./zad3child","./zad3child",argv[2]); exit(EXIT_FAILURE);}
    else if (child > 0) parent_is_coming();
    else fprintf(stderr,"Forking went wrong... .\n");

    printf("\nParent: signals sent: %d.\n", sentToChild);
    printf("Parent: signals received from child: %d.\n", receivedFromChild);

    return 0;
}
