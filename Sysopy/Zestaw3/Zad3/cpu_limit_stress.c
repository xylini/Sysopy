//
// Created by Jakub Pajor on 27.03.2018.
//
#include <stdio.h>
#include <sys/resource.h>
#include <stdlib.h>

int main(int argc, char * argv[]){
    if(argc < 2){
        printf("Please give CPU time (sec) to use.\n");
    }
    struct timeval ru_utime;
    struct timeval ru_stime;
    while(1){
        for(int i = 1; i < 100000000; i=i*10/4);
        for(int i = 1; i < 100000000; i=i*10/4);
        for(int i = 1; i < 100000000; i=i*10/4);
        for(int i = 1; i < 100000000; i=i*10/4);
        struct rusage s_usage;
        getrusage(RUSAGE_SELF, &s_usage);
        ru_utime = s_usage.ru_utime;
        ru_stime = s_usage.ru_stime;
        
        if((int)ru_utime.tv_sec + (int)ru_utime.tv_sec > atoi(argv[1])+2) break;
    }
    printf("\ntest time: U-CPU time: %d.%d sec, S-CPU time: %d.%d sec\n\n",
           (int) ru_utime.tv_sec, (int) ru_utime.tv_usec, (int) ru_stime.tv_sec, (int) ru_stime.tv_usec);

    return 0;
}
