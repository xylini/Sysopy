//
// Created by Jakub Pajor on 24.03.2018.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define ARG_MAX_C 10
#define ARG_MAX_L 255

void create_args(char * line, char * args_tab[ARG_MAX_L]){
    int i = 0;
    while((args_tab[i++] = strtok(i == 0 ? line : NULL, " \n")) != NULL && i < ARG_MAX_L);
    while(i<ARG_MAX_C) args_tab[i++] = NULL;
}

void run_programs(char * file_path){
    FILE * file = fopen(file_path,"r+");
    if(file == NULL) { perror(NULL); exit(EXIT_FAILURE); }
    char line[ARG_MAX_L];
    pid_t pid;
    int status;
    char line_copy[ARG_MAX_L];
    
    printf("Program PID: %d\n",(int)getpid());
    
    while(fgets(line,ARG_MAX_L,file) != NULL){
        char * args_tab[ARG_MAX_L];
        strncpy(line_copy,line,ARG_MAX_L);
        create_args(line,args_tab);
        pid = fork();
        if(pid < 0){perror("fork"); exit(EXIT_FAILURE);}
        if(pid == 0){
            printf("\nCurrent PID: %d\n",(int)getpid());
            status = execvp(args_tab[0],args_tab);
            exit(EXIT_FAILURE);
        }
        wait(&status);
        if(status){
            printf("Błąd wykonania polecenia: %s\n",line_copy);
            exit(EXIT_FAILURE);
        }
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
