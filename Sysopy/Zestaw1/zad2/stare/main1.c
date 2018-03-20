//
// Created by Jakub Pajor on 11.03.2018.
//
#include "charray.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <zconf.h>
#include <sys/times.h>

char* generateRandomString(int maxSize){
    if (maxSize < 1) return NULL;
    char *base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    size_t baseLength = strlen(base);
    char *newString = (char *) malloc((maxSize) * sizeof(char));
    int newStringLength = maxSize - (rand() % maxSize);

    for (int i = 0; i < newStringLength; ++i) {
        newString[i] = base[rand()%baseLength];
    }
    for (int i = newStringLength; i < maxSize + 1; i++){
        newString[i] = '\0';
    }
    return newString;
}
/*
Charray * create_char_array(Charray * array, int array_length, int is_static);                  - line length
Charray * resize_char_array(Charray * array, int new_array_length, int is_static);              - new length
Charray * delete_char_array(Charray * array, int is_static);                                    - nthng
Charray * create_char_block(Charray * array, int index, char * block_content, int is_static);   - index_start, index_stop -> or just length
Charray * delete_char_block(Charray * array, int index, int is_static);                         - index_start, index_stop -> or just length
Charray * change_char_block(Charray * array, int index, char * new_block_content, int is_static); - index_start, index_stop -> or just length
 int closest_bsum_index(Charray * array, int value);                                            - string
 */


int chars_to_number(char * table_size){
    int number = atoi(table_size);
    if(number == 0 && table_size[0] != '0'){
        fprintf(stderr,"chars_to_number: Given Charray/Block length is not number or sth went wrong.");
        exit(EXIT_FAILURE);
    }
    return number;
}

int search_closest(Charray * charray, char * char_string){
    int summ = sum_char_block(char_string);
    int closest = closest_bsum_index(charray,summ);
    return closest;
}

Charray * create_and_delete(Charray * charray, int length, int times, int max_block_length, int is_static){
    for(int i = 0; i < times; i++){
        for(int j = 0; j < length; j++){
            charray = change_char_block(charray,i,generateRandomString(max_block_length),is_static);
        }
    }
    return charray;
}

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

Charray * create_charray_and_block(Charray * charray, int table_size, int block_max_size, int is_static){
    charray = create_char_array(charray,table_size,is_static);
    for(int i = 0; i < table_size; i++){
        char * string_to_give = generateRandomString(block_max_size);
        create_char_block(charray,i,string_to_give,is_static);
        free(string_to_give);
    }
    return charray;
}

int main(int argc, char **argv) {
    srand((unsigned int)time(NULL));
    if(argc < 4) {
        printf("The arguments should be: "
                       "\n\tis_static table_length block_length length_to_create_and_delete times_to_create_and_delete");
        exit(EXIT_FAILURE);
    }


    int is_static = chars_to_number(argv[1]);
    int table_size = chars_to_number(argv[2]);
    int max_block_size = chars_to_number(argv[3]);
    int length_to_del = chars_to_number(argv[4]);
    int times_repeat = chars_to_number(argv[5]);
    char * char_to_find_closest = generateRandomString(max_block_size);

    struct tms **tms_time = malloc(8 * sizeof(struct tms *));
    clock_t real_time[8];
    for (int i = 0; i < 8; i++) {
        tms_time[i] = (struct tms *) malloc(sizeof(struct tms *));
    }

    Charray * charray = NULL;
    real_time[0] = times(tms_time[0]);
        charray = create_charray_and_block(charray, table_size,max_block_size, is_static);
    real_time[1] = times(tms_time[1]);


    printf("Table creation: (%d == size) and block(%d == max_size)\n",table_size,max_block_size);
    printf("real time: %lf   \n", calculate_time(real_time[0], real_time[1]));
    printf("user time: %lf   \n", calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime));
    printf("system time: %lf \n", calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
    printf("\n");

    real_time[2] = times(tms_time[2]);
        search_closest(charray,char_to_find_closest);
    real_time[3] = times((tms_time[3]));

    printf("%s", "Searching: \n");
    printf("real time: %lf   \n", calculate_time(real_time[2], real_time[3]));
    printf("user time: %lf   \n", calculate_time(tms_time[2]->tms_utime, tms_time[3]->tms_utime));
    printf("system time: %lf \n", calculate_time(tms_time[2]->tms_stime, tms_time[3]->tms_stime));
    printf("\n");

    real_time[6] = times(tms_time[6]);
    create_and_delete(charray,length_to_del,1,max_block_size,is_static);
    real_time[7] = times(tms_time[7]);

    printf("Single create and delete (%d == length) \n",length_to_del);
    printf("real time: %lf   \n", calculate_time(real_time[6], real_time[7]));
    printf("user time: %lf   \n", calculate_time(tms_time[6]->tms_utime, tms_time[7]->tms_utime));
    printf("system time: %lf \n", calculate_time(tms_time[6]->tms_stime, tms_time[7]->tms_stime));
    printf("\n");

    real_time[4] = times(tms_time[4]);
        create_and_delete(charray,length_to_del,times_repeat,max_block_size,is_static);
    real_time[5] = times(tms_time[5]);

    printf("Create and delete (%d == times, %d == length) \n",times_repeat,length_to_del);
    printf("real time: %lf   \n", calculate_time(real_time[4], real_time[5]));
    printf("user time: %lf   \n", calculate_time(tms_time[4]->tms_utime, tms_time[5]->tms_utime));
    printf("system time: %lf \n", calculate_time(tms_time[4]->tms_stime, tms_time[5]->tms_stime));
    printf("+------------------------------------+\n");



    return 0;
}
