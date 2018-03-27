//
// Created by Jakub Pajor on 27.03.2018.
//
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]){
    if(argc < 2){
        printf("Please give memory size (MB) to allocate.\n");
    }
    int a_size = atoi(argv[1]) * 1000000; // in MB
    char * stress = malloc(a_size * sizeof(char));
    for(int i = 0; i < a_size; i++){
        stress[i] = 'a';
    }
}
