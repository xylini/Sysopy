//
// Created by Jakub Pajor on 08.03.2018.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "charray.h"

#define s_Charray_MAX_LENGTH  10000
#define s_Charray_MAX_BLOCK  10000
Charray s_charray[s_Charray_MAX_LENGTH];
char char_table[s_Charray_MAX_LENGTH][s_Charray_MAX_BLOCK];

Charray *create_char_array(Charray *array, int array_length, int is_static) {
    if (is_static != 1 && is_static != 0) {
        fprintf(stderr, "create_char_array: is_static could be 1(is) or 0(is not)");
        exit(EXIT_FAILURE);
    }
    if (array_length > s_Charray_MAX_LENGTH && is_static == 1) {
        fprintf(stderr, "create_char_array: The array you are trying to create is too long. Max size: %d.\n",
                s_Charray_MAX_LENGTH);
        exit(EXIT_FAILURE);
    }

    if (array != NULL && is_static == 0) {
        fprintf(stderr, "create_char_array: The array is already allocated with length %d or is not NULL when trying to create array.\n", array[0].length);
        exit(EXIT_FAILURE);
    }

    if (is_static == 1) array = s_charray;

    if (is_static == 0) {
        array = calloc(array_length, sizeof(Charray));
        if (array == NULL) {
            fprintf(stderr, "create_char_array: No memory available.\n");
            exit(EXIT_FAILURE);
        }
    }

    array[0].length = array_length;
    for (int i = 0; i < array_length; i++) {
        array[i].block_length = 0;
        array[i].block_sum = (int)NULL;
        array[i].char_block = NULL;
    }
    return array;
}


Charray *resize_char_array(Charray *array, int new_array_length, int is_static) {
    if (is_static != 1 && is_static != 0) {
        fprintf(stderr, "resize_char_array: is_static could be 1(is) or 0(is not)");
        exit(EXIT_FAILURE);
    }

    Charray *newArray;
    if (is_static == 0) {
        newArray = realloc(array, new_array_length * sizeof(Charray));
        if (newArray == NULL) {
            fprintf(stderr, "resize_char_array: No memory available.\n");
            exit(EXIT_FAILURE);
        }
        array = newArray;
    }
    if (is_static == 1 && new_array_length > s_Charray_MAX_LENGTH) {
        fprintf(stderr, "resize_char_array: The length you are trying to add is too much. \n"
                "\tMax length: %d, current length: %d.", s_Charray_MAX_LENGTH, array[0].length);
        exit(EXIT_FAILURE);
    }

    for (int i = array[0].length; i < new_array_length; i++) {
        array[i].block_length = 0;
        array[i].block_sum = (int)NULL;
        array[i].char_block = NULL;
    }
    array[0].length = new_array_length;
    return array;
}

Charray *delete_char_array(Charray *array, int is_static) {
    if (is_static != 1 && is_static != 0) {
        fprintf(stderr, "delete_char_array: is_static could be 1(is) or 0(is not)");
        exit(EXIT_FAILURE);
    }
    if (array == NULL) {
        fprintf(stderr, "delete_char_array: Given pointer is NULL.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < array[0].length; i++) {
        delete_char_block(array, i, is_static);
    }
    array[0].length = 0;
    if (is_static == 0) free(array);
    array = NULL;
    return array;

}


int sum_char_block(char *char_block) {
    int block_sum = 0;
    int char_block_length = (unsigned int)strlen(char_block);

    for(int i = 0; i < char_block_length; i++){
        block_sum += char_block[i];
    }
    return block_sum;
}

Charray *create_char_block(Charray *array, int index, char *block_content, int is_static) {
    if (is_static != 1 && is_static != 0) {
        fprintf(stderr, "create_char_block: is_static could be 1(is) or 0(is not)");
        exit(EXIT_FAILURE);
    }

    if (array == NULL) {
        fprintf(stderr, "create_char_block: Given array pointer is NULL.\n");
        exit(EXIT_FAILURE);
    }
    if (index >= array[0].length) {
        printf("%d", array[0].length);
        fprintf(stderr,
                "create_char_block: Given index in Charray * is beyond the range. Use resize_char_array function.\n");
        exit(EXIT_FAILURE);
    }
    if (array[index].char_block != NULL) {
        fprintf(stderr,
                "create_char_block: Charray * was allocated inappropriately or given block index is already allocated(!= NULL)."
                        "\n\t Use create_char_array or change_char_array function.\n");
        exit(EXIT_FAILURE);
    }
    if (array[index].block_length != 0) {
        fprintf(stderr, "create_char_block: Charray * is already allocated.\n");
        exit(EXIT_FAILURE);
    }

    int block_length = strlen(block_content);
    if (is_static == 1 && block_length >= s_Charray_MAX_BLOCK) {
        fprintf(stderr, "create_char_block: The length of block is too long. Max length: %d.\n",
                s_Charray_MAX_BLOCK - 1);
        exit(EXIT_FAILURE);
    }

    if (is_static == 0) array[index].char_block = calloc(block_length + 1, sizeof(char));
    else array[index].char_block = char_table[index];

    strcpy(array[index].char_block, block_content);
    array[index].char_block[block_length] = '\0';
    array[index].block_length = block_length;
    array[index].block_sum = sum_char_block(array[index].char_block);
    return array;
}

Charray *delete_char_block(Charray *array, int index, int is_static) {
    if (is_static != 1 && is_static != 0) {
        fprintf(stderr, "delete_char_block: is_static could be 1(is) or 0(is not)");
        exit(EXIT_FAILURE);
    }
    if (array == NULL) {
        fprintf(stderr, "delete_char_block: Given array pointer is NULL.\n");
        exit(EXIT_FAILURE);
    }
    if (index >= array[0].length) {
        fprintf(stderr, "delete_char_block: Given index does not point to available block.\n");
        exit(EXIT_FAILURE);
    }

    if (is_static == 0) free(array[index].char_block);
    else if (array[index].char_block != NULL) array[index].char_block[0] = '\0';
    array[index].block_length = 0;
    array[index].char_block = NULL;
    array[index].block_sum = (int)NULL;
    return array;

}

Charray *change_char_block(Charray *array, int index, char *new_block_content, int is_static) {
    if (is_static != 1 && is_static != 0) {
        fprintf(stderr, "change_char_block: is_static could be 1(is) or 0(is not)");
        exit(EXIT_FAILURE);
    }
    array = delete_char_block(array, index, is_static);
    array = create_char_block(array, index, new_block_content, is_static);
    return array;

}

int closest_bsum_index(Charray *array, int value) {
    int index = 0;
    int difference = INT_MAX;
    int abs_diff=0;
    for (int i = 0; i < array[0].length; i++) {
        abs_diff = abs(value - array[i].block_sum);
        if (abs_diff < difference) {
            difference = abs_diff;
            index = i;
        }
    }

    return index;
}

void printCharray(Charray *array) {
    for (int i = 0; i < array[0].length; i++) {
        printf("%s\n", array[i].char_block);
    }
}

