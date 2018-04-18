//
// Created by Jakub Pajor on 17.04.2018.
//
#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <string.h>
#include <sys/stat.h>


int main(int argc, char * argv[]){
    mkfifo(argv[1], S_IWUSR | S_IRUSR);
    char buffer[512];
    FILE * fifopipo;
    //printf("Master started, PID - %d.\n",getpid());
    if((fifopipo = fopen(argv[1], "r")) == NULL){ printf("Error while opening FIFO.\n"); exit(EXIT_FAILURE);}
    while(fgets(buffer, 512, fifopipo) != NULL){ write(1, buffer, strlen(buffer));}
    fclose(fifopipo);
    return 0;
}