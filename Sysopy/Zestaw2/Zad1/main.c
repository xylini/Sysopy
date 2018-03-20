#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/times.h>
//#include <zconf.h>
#include <time.h>

char* generateRandomString(int max_size) {
    if (max_size < 1) return NULL;
    char *base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    size_t dict_len = strlen(base);
    char *res = (char *) malloc((max_size) * sizeof(char));
    
    for (int i = 0; i < max_size-1; i++) {
        res[i] = base[rand() % dict_len];
    }
    res[max_size-1] = 13;
    
    return res;
}

void s_generate(char * file_name, int length, int records){
    FILE * file = fopen(file_name,"w+");
    if(file == NULL) {
        perror(file_name);
        exit(EXIT_FAILURE);
    }
    char rand_data[length];
    for(int i = 0; i < records;i++) {
        strcpy(rand_data, generateRandomString(length));
        fputs(rand_data, file);
    }
    fclose(file);
}

void generate(char * file_name, int length, int records){
    FILE * file = fopen(file_name,"w+");
    if(file == NULL) { perror("file"); exit(EXIT_FAILURE); }
    int char_length = length * records;
    char * rand_data = malloc(char_length * sizeof(char));
    arc4random_buf(rand_data, (size_t) (char_length));
    fwrite(rand_data, sizeof(char),(size_t) (char_length),file);
    free(rand_data);
    fclose(file);
}

void lib_sort(char * file_name, int length, int records){ //zrobic obsluge bledow wykonania
    char * first_rec = malloc(length * sizeof(char));
    char * sec_rec = malloc(length * sizeof(char));
    long int offset_shift = (long int) (length * sizeof(char));
    FILE * file = fopen(file_name,"r+");
    if(file == NULL) { perror(NULL); exit(EXIT_FAILURE); }
    
    for(int i = 1; i < records; i++){
        for(int j = i-1; j >= 0; j--){
            fseek(file,(j+1) * offset_shift,0);
            if(fread(first_rec, sizeof(char), (size_t) length,file) != length) exit(EXIT_FAILURE);
            
            fseek(file,j * offset_shift,0);
            if(fread(sec_rec, sizeof(char),(size_t) length,file) != length) exit(EXIT_FAILURE);
            
            if(sec_rec[0] > first_rec[0]){
                fseek(file,(j+1) * offset_shift,0);
                if(fwrite(sec_rec, sizeof(char),(size_t) length,file) != length) exit(EXIT_FAILURE);
                fseek(file,j * offset_shift,0);
                if(fwrite(first_rec, sizeof(char),(size_t) length,file) != length) exit(EXIT_FAILURE);
            }
        }
    }
    fclose(file);
    free(first_rec);
    free(sec_rec);
}

void sys_sort(char * file_name, int length, int records){
    char * first_rec = malloc(length * sizeof(char));
    char * sec_rec = malloc(length * sizeof(char));
    int file = open(file_name,O_RDWR);
    if(file == -1) { perror(NULL); exit(EXIT_FAILURE); }
    
    for(int i = 1; i < records; i++){
        for(int j = i-1; j >= 0; j--){
            lseek(file,(j+1) * length,SEEK_SET);
            if(read(file,first_rec, (size_t) length) != length) exit(EXIT_FAILURE);
            
            lseek(file,j * length,SEEK_SET);
            if(read(file,sec_rec, (size_t) length) != length) exit(EXIT_FAILURE);
            
            if(sec_rec[0] > first_rec[0]){
                lseek(file,(j+1) * length, SEEK_SET);
                if(write(file, sec_rec, (size_t) length) != length) exit(EXIT_FAILURE);
                lseek(file, j * length, SEEK_SET);
                if(write(file, first_rec, (size_t) length) != length) exit(EXIT_FAILURE);
            }
        }
    }
    close(file);
    free(first_rec);
    free(sec_rec);
}

void lib_copy(char * source_file_name, char * dest_file_name, int length, int records, int buffor_size){
    FILE * source_file = fopen(source_file_name,"r");
    FILE * dest_file = fopen(dest_file_name,"w+");
    char * buffor = malloc(buffor_size * sizeof(char));
    
    if(source_file == NULL || dest_file == NULL) { perror(NULL); exit(EXIT_FAILURE); }
    int size_of_last_to_be_read = (length * records) % buffor_size;
    
    for(int i = 0; i * buffor_size < (length * records)-size_of_last_to_be_read; i++) {
        if(fread(buffor, sizeof(char), (size_t) buffor_size, source_file) != buffor_size)
        {perror(source_file_name); exit(EXIT_FAILURE);}
        if(fwrite(buffor, sizeof(char), (size_t) buffor_size,dest_file) != buffor_size)
        {perror(dest_file_name); exit(EXIT_FAILURE);}
    }
    
    if(size_of_last_to_be_read != 0){
        if(fread(buffor, sizeof(char), (size_t) size_of_last_to_be_read, source_file) != size_of_last_to_be_read)
        {perror(source_file_name); exit(EXIT_FAILURE);}
        if(fwrite(buffor, sizeof(char), (size_t) size_of_last_to_be_read, dest_file) != size_of_last_to_be_read)
        {perror(dest_file_name); exit(EXIT_FAILURE);}
    }
    fclose(source_file);
    fclose(dest_file);
    free(buffor);
}

void sys_copy(char * source_file_name, char * dest_file_name, int length, int records, int buffor_size){
    int source_file = open(source_file_name, O_RDONLY);
    
    //only write, create if doesn't exists, writing on the end of file, if exists -> file length = 0
    int dest_file = open(dest_file_name, O_WRONLY | O_CREAT | O_APPEND| O_TRUNC,0644);//0644 - u:wr,g:r,p:r
    char * buffor = malloc(buffor_size * sizeof(char));
    
    if(source_file == -1 || dest_file == -1)
        { perror(NULL); exit(EXIT_FAILURE); }
    
    int size_of_last_to_be_read = (length * records) % buffor_size;
    
    for(int i = 0; i * buffor_size < (length * records)-size_of_last_to_be_read; i++) {
        if(read(source_file,buffor,(size_t) buffor_size) != buffor_size)
            {perror(source_file_name); printf("dupa"); exit(EXIT_FAILURE);}
        if(write(dest_file,buffor,(size_t) buffor_size) != buffor_size)
            {perror(dest_file_name); exit(EXIT_FAILURE);}
    }
    
    if(size_of_last_to_be_read != 0) {
        if(read(source_file, buffor, (size_t) size_of_last_to_be_read) != size_of_last_to_be_read)
        {perror(source_file_name); exit(EXIT_FAILURE);}
        if(write(dest_file, buffor, (size_t) size_of_last_to_be_read) != size_of_last_to_be_read)
        {perror(dest_file_name); exit(EXIT_FAILURE);}
    }
    close(dest_file);
    close(source_file);
    free(buffor);
}

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void sort(char * resource_type, char * file_name, int length, int records){
    if(strcmp(resource_type,"sys") == 0)
        sys_sort(file_name, length, records);
    else if(strcmp(resource_type, "lib") == 0)
        lib_sort(file_name, length, records);
    else {
        fprintf(stderr,"sort: Bad option. Use: sys or lib.\n");
        exit(EXIT_FAILURE);
    }
}

void copy(char * resource_type, char * source_file_name, char * dest_file_name, int length, int records, int buffor_size){
    if(strcmp(resource_type,"sys") == 0)
        sys_copy(source_file_name,dest_file_name,length,records,buffor_size);
    else if(strcmp(resource_type, "lib") == 0)
        lib_copy(source_file_name,dest_file_name,length,records,buffor_size);
    else {
        fprintf(stderr,"copy: Bad option. Use: sys or lib.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv){
    struct tms **tms_time = malloc(5 * sizeof(struct tms *));
    clock_t real_time[5];
    for (int i = 0; i < 5; i++) {
        tms_time[i] = (struct tms *) malloc(sizeof(struct tms *));
    }
    if(argc < 4 || argc > 8){
        printf("Wrong arguments.\n"
               "Please use one of: \n"
               "1) generate [file_name] [records] [length]\n"
               "2) s_generate [file_name] [records] [length]\n"
               "3) sort [file_name] [records] [length] [sys or lib]\n"
               "4) copy [source_file_name] [dest_file_name] [records] [length] [buffor_size] [sys or lib]\n");
    }
    else{
        
        
        if(strcmp(argv[1],"generate") == 0){
            srand((unsigned int) time(NULL));
            generate(argv[2],(int) strtol(argv[4],NULL,10), (int) strtol(argv[3],NULL,10));
        }
        if(strcmp(argv[1],"s_generate") == 0){
            s_generate(argv[2],(int) strtol(argv[4],NULL,10), (int) strtol(argv[3],NULL,10));
        }
        if(strcmp(argv[1],"sort") == 0){
            real_time[0] = times(tms_time[0]);
            sort(argv[5],argv[2],(int) strtol(argv[4],NULL,10),(int) strtol(argv[3],NULL,10));
            real_time[1] = times(tms_time[1]);
            
            printf("Sorting time. Used library: %s\n",argv[5]);
            printf("real time: %lf   ", calculate_time(real_time[0], real_time[1]));
            printf("user time: %lf   ", calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime));
            printf("system time: %lf ", calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
            printf("\n");
        }
        if(strcmp(argv[1],"copy") == 0){
            real_time[2] = times(tms_time[2]);
            copy(argv[7],argv[2],argv[3],(int) strtol(argv[5],NULL,10),(int) strtol(argv[4],NULL,10),(int) strtol(argv[6],NULL,10));
            real_time[3] = times(tms_time[3]);
            
            printf("Copying time. Used library: %s\n",argv[7]);
            printf("real time: %lf   ", calculate_time(real_time[2], real_time[3]));
            printf("user time: %lf   ", calculate_time(tms_time[2]->tms_utime, tms_time[3]->tms_utime));
            printf("system time: %lf ", calculate_time(tms_time[2]->tms_stime, tms_time[3]->tms_stime));
            printf("\n");
        }
    }
    
    return 0;
}
