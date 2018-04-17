//
// Created by Jakub Pajor on 17.04.2018.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define ARG_MAX_C 10
#define ARG_MAX_L 512
#define PIPE_ARG_MAX 10

void create_args(char * line, char * args_tab[ARG_MAX_L]){
    int i = 0;
    while((args_tab[i] = strtok(i == 0 ? line : NULL, " \t")) != NULL && i++ < ARG_MAX_L);
    while(i<ARG_MAX_C) args_tab[i++] = NULL;
}

int split_line_into_progs(char * line, char * pipe_programs[ARG_MAX_L]){
    int i = 0;
    int progs;
    while((pipe_programs[i] = strtok(i == 0 ? line : NULL, "|\n")) != NULL && i++ < ARG_MAX_L);
    progs = i;
    while(i<PIPE_ARG_MAX) pipe_programs[i++] = NULL;
    return progs;
}

void run_line(int progs_in_line, char * pipe_programs[ARG_MAX_L]){
    int fd[2][2], i, status;
    pid_t pid;
    for(i = 0; i < progs_in_line; i++){
        char * args_tab[ARG_MAX_L];
        create_args(pipe_programs[i],args_tab);


        if(i > 0){
            close(fd[i % 2][0]);
            close(fd[i % 2][1]);
        }
        if(pipe(fd[i % 2]) == -1){perror("pipe"); exit(EXIT_FAILURE);}

        pid = fork();
        if(pid == 0){
            printf("\nCurrent PID: %d\n",(int)getpid());
            if(i != progs_in_line - 1){
                close(fd[i % 2][0]);
                if(dup2(fd[i % 2][1], STDOUT_FILENO) < 0) exit(EXIT_FAILURE);
            }
            if(i != 0){
                close(fd[(i + 1) % 2][1]);
                if(dup2(fd[(i + 1)%2][0],STDIN_FILENO) < 0) exit(EXIT_FAILURE);
            }
            status = execvp(args_tab[0],args_tab); exit(EXIT_FAILURE);
        }
    }
    wait(&status);
    close(fd[i % 2][0]);
    close(fd[i % 2][1]);
}


void run_programs(char * file_path){
    FILE * file = fopen(file_path,"r+");
    if(file == NULL) { perror(NULL); exit(EXIT_FAILURE); }
    char line[ARG_MAX_L];
    pid_t pid;

    printf("Program PID: %d\n",(int)getpid());
    while(fgets(line,ARG_MAX_L,file) != NULL){
        if(strcmp(line,"\n") == 0) continue;
        char * pipe_programs[ARG_MAX_L];
        int progs_in_line = split_line_into_progs(line,pipe_programs);
        int status;
        pid = fork();
        if(pid == 0) {run_line(progs_in_line,pipe_programs); exit(EXIT_SUCCESS);}
        status = waitpid(pid,NULL,0);
        if (status == -1) {perror("fork"); exit(EXIT_FAILURE); }
    }
    fclose(file);
}


int main(int argc, char * argv[]){
    if(argc < 2){
        printf("Please write a path to file to execute it's commands as first argument.\n");
        exit(EXIT_FAILURE);
    }
    run_programs(argv[1]);


    return 0;
}