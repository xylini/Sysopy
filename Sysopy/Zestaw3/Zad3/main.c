//
// Created by Jakub Pajor on 25.03.2018.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <ulimit.h>

#define ARG_MAX_C 10
#define ARG_MAX_L 255

void create_args(char * line, char * args_tab[ARG_MAX_L]){
    int i = 0;
    while((args_tab[i++] = strtok(i == 0 ? line : NULL, " \n")) != NULL && i < ARG_MAX_L);
    while(i<ARG_MAX_C) args_tab[i++] = NULL;
}

void print_time(struct rusage f_usage){ //f_usage -> first usage -> to get actual time
    struct rusage s_usage;
    getrusage(RUSAGE_CHILDREN, &s_usage);
    struct timeval ru_utime;
    struct timeval ru_stime;
    timersub(&s_usage.ru_utime, &f_usage.ru_utime, &ru_utime);
    timersub(&s_usage.ru_stime, &f_usage.ru_stime, &ru_stime);
    f_usage = s_usage;
    
    printf("\nU-CPU time: %d.%d sec, S-CPU time: %d.%d sec\n\n",
           (int) ru_utime.tv_sec, (int) ru_utime.tv_usec, (int) ru_stime.tv_sec, (int) ru_stime.tv_usec);
    /* tv_sec - seconds, tv_usec - microseconds */
}

void run_programs(char * file_path, rlim_t cpu_limit, rlim_t mem_limit){
    FILE * file = fopen(file_path,"r+");
    if(file == NULL) { perror(NULL); exit(EXIT_FAILURE); }
    char line[ARG_MAX_L];
    pid_t pid;
    int status;
    char line_copy[ARG_MAX_L];
    
    printf("Program PID: %d\n",(int)getpid());
    
    struct rusage f_usage;
    
    while(fgets(line,ARG_MAX_L,file) != NULL){
        char * args_tab[ARG_MAX_L];
        strncpy(line_copy,line,ARG_MAX_L);
        create_args(line,args_tab);
        
        getrusage(RUSAGE_CHILDREN, &f_usage);
        pid = fork();
        if(pid < 0){perror("fork"); exit(EXIT_FAILURE);}
        if(pid == 0){
            printf("\nCurrent PID: %d\n",(int)getpid());
            
            struct rlimit cpu_limit_var;
            cpu_limit_var.rlim_max = cpu_limit;
            cpu_limit_var.rlim_cur = cpu_limit;
            
            struct rlimit mem_limit_var;
            mem_limit_var.rlim_max = mem_limit;
            mem_limit_var.rlim_cur = mem_limit;
            
            setrlimit(RLIMIT_AS, &mem_limit_var);
            setrlimit(RLIMIT_CPU, &cpu_limit_var);
            
            /*
             struct rlimit memLimitData1;
             getrlimit(RLIMIT_CPU, &memLimitData1);
             printf("%d\n",memLimitData1.rlim_max);
             */
            status = execvp(args_tab[0],args_tab);
            exit(EXIT_FAILURE);
        }
        wait(&status);
        if(status){
            printf("Couldn't execute this line: %s\n",line_copy);
            exit(EXIT_FAILURE);
        }
        print_time(f_usage);
    }
    fclose(file);
}

void parse_arguments(int argc, char ** argv, char ** file_path, rlim_t * cpu_limit, rlim_t * mem_limit){
    if(argc < 4){ printf("Not enough argmuents, write: [file_path] [CPU limit] [memory_limit].\n");exit(EXIT_FAILURE); }
    
    *file_path = argv[1];
    char * buf;
    
    *cpu_limit = (rlim_t) strtol(argv[2], &buf, 10);
    if(*buf != '\0'){ printf("CPU limit conversion error.\n");exit(EXIT_FAILURE);}
    
    *mem_limit = (rlim_t) strtol(argv[3], &buf, 10);
    if(*buf != '\0'){ printf("Memory limit conversion error.\n");exit(EXIT_FAILURE);}
    *mem_limit *= 1000000;
}


int main(int argc, char * argv[]){
    char * path;
    rlim_t cpu_limit;
    rlim_t mem_limit;
    parse_arguments(argc,argv,&path,&cpu_limit,&mem_limit);
    
    run_programs(path,cpu_limit, mem_limit);
    return 0;
}
