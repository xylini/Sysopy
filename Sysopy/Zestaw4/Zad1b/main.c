//
// Created by Jakub Pajor on 08.04.2018.
//

#include <stdio.h>
#include <zconf.h>
#include <signal.h>
#include <stdlib.h>

int freezed = 0;
pid_t child_pid = -1;

void SIGINT_handling(int signum){
        if(child_pid != -1) kill(child_pid,SIGKILL);
        printf("\nSIGINT received. Finito amigo.\n");
        exit(EXIT_SUCCESS);
}

void SIGTSTP_handling(int sig_no){
    if(!freezed)        printf("\nWaiting for CTRL + Z or CTRL + C.\n");
    if(child_pid != -1) kill(child_pid,SIGKILL);
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
        int status;
        if(freezed != 1)
            if((child_pid = fork()) == 0){ status = execvp("./script.sh",argv); exit(EXIT_FAILURE);}
        wait(&status);
    }

    return 0;
}