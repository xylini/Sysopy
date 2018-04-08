#include <stdio.h>
#include <zconf.h>
#include <signal.h>
#include <stdlib.h>

int freezed = 0;

void SIGINT_handling(int signum){
        printf("\nSIGINT received. Finito amigo.\n");
        exit(EXIT_SUCCESS);
}

void SIGTSTP_handling(int sig_no){
    if(!freezed) printf("\nWaiting for CTRL + Z or CTRL + C.\n");
    freezed = freezed == 0 ? 1 : 0;
}


int main(int argc, char * argv[]) {
    struct sigaction action;
    action.sa_handler = SIGTSTP_handling;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGTSTP, &action, NULL);

    signal(SIGINT,SIGINT_handling);

    while(1){
        if(freezed != 1)
            if((int) fork() == 0) execvp("date",argv);
        sleep(1);
    }

    return 0;
}