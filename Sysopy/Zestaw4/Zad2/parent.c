//
// Created by Jakub Pajor on 09.04.2018.
//


#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define catcher_init(sig, func){										\
	action.sa_sigaction = &(func); 										\
    	if(sigaction((sig), &action, NULL) == -1) fprintf(stderr, "Sth went wrong. Cannot catch %d.\n",sig); 	\
}

int M;      //  SIGUSR1 number signals before letting then start
int N;      //  children number
volatile int el_before_start;
volatile int children_left = 0;
pid_t * children;
pid_t * please_let_me_go;

volatile int returns_received = 0;
volatile int SIGRT_received = 0;



void int_handler(int sig_no, siginfo_t *siginfo, void *context){
    printf("\rSIGINT received. Killing kids. Suicide.\n");
    for(int i = 0; i < N; i++)
        if(children[i] != 0)
            {kill(children[i],SIGKILL); waitpid(children[i],NULL,0);}
    free(children);
    exit(EXIT_SUCCESS);
}

void usr1_handler(int sig_no, siginfo_t *siginfo, void *context){
    if(el_before_start > 0){
        --el_before_start;
        printf("SIGUSR1 received form chld: %d, signals left to start: %d\n",siginfo->si_pid,el_before_start);
        please_let_me_go[el_before_start] = siginfo->si_pid;
        if(el_before_start == 0){
            for(int i = M-1; i >=0; --i) kill(please_let_me_go[i],SIGUSR2);
        }
    }
    else{
        printf("SIGUSR1 received form chld: %d, starting it immediately.\n",siginfo->si_pid);
        kill(siginfo->si_pid,SIGUSR2);
    }
}


void rt_handler(int sig_no, siginfo_t *siginfo, void *context){
    fprintf(stderr,"\tSIGRT nr: %d received from chld: %d.\n",sig_no,siginfo->si_pid);
    SIGRT_received++;
}

void chld_handler(int sig_no, siginfo_t *siginfo, void *context){
    printf("Chld %d finished job with return: %d\n",siginfo->si_pid,siginfo->si_status);
    --children_left;
    ++returns_received;
}


int main(int argc, char * argv[]) {
    if(argc > 2){
        M = atoi(argv[1]);
        N = atoi(argv[2]);
    }

    if(M > N){
        printf("Cannot wait for more signals than have children.\n");
        exit(EXIT_FAILURE);
    }
    el_before_start = M;
    children = calloc(N, sizeof(pid_t));
    please_let_me_go = calloc(M, sizeof(pid_t));


    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;

	
    catcher_init(SIGINT, int_handler);
    catcher_init(SIGUSR1, usr1_handler);
    catcher_init(SIGCHLD, chld_handler);

    for(int i = SIGRTMIN; i <= SIGRTMAX; i++)
        catcher_init(i, rt_handler);


    for(int i = 0; i < N; i++){
        children[i] = fork();
        if(children[i] == 0) { execl("./child","./child",NULL); exit(EXIT_FAILURE); }
        children_left++;
    }




    while(children_left && SIGRT_received != N);
    sleep(1);
    printf("\n\nChildren returns received: %d, children SIGRTs: %d\n",N-children_left, SIGRT_received);
    return 0;
}
