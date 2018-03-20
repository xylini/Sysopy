//
// Created by Jakub Pajor on 08.03.2018.
//

#ifndef SYSOPS_CHARRAY_H
#define SYSOPS_CHARRAY_H

typedef struct Charray{
    int length;
    int block_length;
    int block_sum;
    char * char_block;
}Charray;



Charray * create_char_array(Charray * array, int array_length, int is_static);
Charray * resize_char_array(Charray * array, int new_array_length, int is_static);
Charray * delete_char_array(Charray * array, int is_static);
int sum_char_block(char * char_block);
Charray * create_char_block(Charray * array, int index, char * block_content, int is_static);
Charray * delete_char_block(Charray * array, int index, int is_static);
Charray * change_char_block(Charray * array, int index, char * new_block_content, int is_static);
int closest_bsum_index(Charray * array, int value);
void printCharray(Charray * array);



#endif //SYSOPS_CHARRAY_H
