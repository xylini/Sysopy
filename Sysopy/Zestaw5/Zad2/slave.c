//
// Created by Jakub Pajor on 17.04.2018.
//

#include <zconf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define DATE_LT 128
#define DATA_LT 512

int main(int argc, char * argv[]){
    srand((unsigned int) getpid());
    int N, fifopipo;
    FILE * date;
    if((fifopipo = open(argv[1], O_WRONLY)) == -1) {perror(argv[1]); exit(EXIT_FAILURE);}
    if((N = atoi(argv[2])) == 0) {printf("Wrong argument. Please give a number as argument.\n"); exit(EXIT_FAILURE);}
    printf("Slave: %d, pid: %d\n\n", N, getpid());

    char date_buffer[DATE_LT];
    char result_buffer[DATA_LT];
    for(int i = 0; i < N; i++){
        if((date = popen("date","r")) == NULL) {perror("date"); exit(EXIT_FAILURE);};
        fgets(date_buffer, DATE_LT, date);
        sprintf(result_buffer,"Slave: %d, %s\n",getpid(),date_buffer);
        write(fifopipo, result_buffer, strlen(result_buffer));
        sleep((unsigned int) rand() % 3 + 2);
    }
    close(fifopipo);
    return 0;
}