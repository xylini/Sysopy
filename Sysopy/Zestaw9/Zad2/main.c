#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>


int producers;
int consumers;
int buff_size;
int min_line_length;
int max_work_time;
int search_mode;
int info_mode;

char **buffer;
sem_t *buff_sem;
sem_t buff_avability_sem;
sem_t prod_sem;
sem_t cons_sem;

pthread_t *prod_thread;
pthread_t *cons_thread;

FILE *file;
char * file_name;
int last_prod_index = 0;
int last_cons_index = 0;
int is_writing_done = 0;

void signal_processing(int signo);
void init(char **argv) {
    producers = atoi(argv[1]);
    consumers = atoi(argv[2]);
    buff_size = atoi(argv[3]); // *buffer  size
    file_name = argv[4];
    min_line_length = atoi(argv[5]); // string minimum length
    search_mode = atoi(argv[6]);
    info_mode = atoi(argv[7]);
    max_work_time = atoi(argv[8]); // 0 - until file reading is finished or sig ctrl+c, 0< - secs


    signal(SIGINT, signal_processing);
    if (max_work_time > 0) signal(SIGALRM, signal_processing);

    if ((file = fopen(file_name, "r")) == NULL) {perror(file_name); exit(EXIT_FAILURE);}

    buffer = malloc((size_t) buff_size * sizeof(char *));
    buff_sem = malloc((buff_size) * sizeof(sem_t));

    for (int i = 0; i < buff_size; ++i){
        sem_init(&buff_sem[i],0,1);
        buffer[i] = NULL;
    }
    sem_init(&prod_sem,0,1);
    sem_init(&cons_sem,0,1);
    sem_init(&buff_avability_sem,0,buff_size);
}


void finito_bonito() {
    if (file) fclose(file);

    for (int i = 0; i < buff_size; i++) {
        if (buffer[i]) free(buffer[i]);
        sem_destroy(&buff_sem[i]);
    }
    sem_destroy(&prod_sem);
    sem_destroy(&cons_sem);
    sem_destroy(&buff_avability_sem);
    free(buffer);
    free(buff_sem);
}

void signal_processing(int signo) {
    fprintf(stderr,"Sig: %d received. Stoping the program.\n", signo);
    for (int i = 0; i < producers; i++) pthread_cancel(prod_thread[i]);
    for (int i = 0; i < consumers; i++) pthread_cancel(cons_thread[i]);
    finito_bonito();
    exit(EXIT_SUCCESS);
}

void *producer(void *pVoid) {
    int index;
    char line[LINE_MAX];
    while (fgets(line, LINE_MAX, file) != NULL) {
        if(info_mode) fprintf(stderr, "P: %ld reading a line.\n", (long) pthread_self());
        sem_wait(&prod_sem);
        sem_wait(&buff_avability_sem);

        index = last_prod_index;
        if(info_mode) fprintf(stderr, "P: %ld reading buff index (%d).\n", (long) pthread_self(), index);
        last_prod_index = (last_prod_index + 1) % buff_size;


        sem_wait(&buff_sem[index]);
        sem_post(&prod_sem);

        buffer[index] = malloc((strlen(line) + 1) * sizeof(char));
        strcpy(buffer[index], line);
        if(info_mode) fprintf(stderr, "P: %ld line copied to buffer at index (%d).\n", (long) pthread_self(), index);

        sem_post(&buff_sem[index]);
    }
    if(info_mode) fprintf(stderr, "P: %ld the job is done.\n", (long) pthread_self());
    return NULL;
}

void *consumer(void *pVoid) {
    char *line;
    int index;
    while (1) {
        sem_wait(&cons_sem);

        while (buffer[last_cons_index] == NULL) {
            sem_post(&cons_sem);
            if (is_writing_done) {
                if(info_mode) fprintf(stderr, "C: %ld the job is done. \n", (long) pthread_self());
                return NULL;
            }
            sem_wait(&cons_sem);
        }

        index = last_cons_index;
        if(info_mode) fprintf(stderr, "C: %ld reading buff index (%d).\n", (long) pthread_self(), index);
        last_cons_index = (last_cons_index + 1) % buff_size;

        sem_wait(&buff_sem[index]);

        line = buffer[index];
        buffer[index] = NULL;
        if(info_mode) fprintf(stderr, "C: %ld taking line from buffer at index (%d).\n", (long) pthread_self(), index);

        sem_post(&buff_avability_sem);
        sem_post(&cons_sem);
        sem_post(&buff_sem[index]);

        if(strlen(line) > min_line_length){
            if(info_mode) fprintf(stderr, "C: %ld there is appropriate line with length %d %c %d.\n",
                                  (long) pthread_self(), (int) strlen(line), search_mode == 1 ? '>' : search_mode == -1 ? '<' : '=', min_line_length);
            fprintf(stderr, "%ld - \t%d\t- %s", (long) pthread_self(), index, line);
        }
        free(line);
         usleep(10);
    }
}

void start_threads() {
    prod_thread = malloc(producers * sizeof(pthread_t));
    cons_thread = malloc(consumers * sizeof(pthread_t));

    for (int i = 0; i < producers; i++) pthread_create(&prod_thread[i], NULL, producer, NULL);
    for (int i = 0; i < consumers; i++) pthread_create(&cons_thread[i], NULL, consumer, NULL);

    if (max_work_time > 0) alarm((unsigned int) max_work_time);

    for (int i = 0; i < producers; i++) pthread_join(prod_thread[i], NULL);

    is_writing_done = 1;

    for (int i = 0; i < consumers; i++) pthread_join(cons_thread[i], NULL);
}


int main(int argc, char **argv) {
    if (argc < 9){
        fprintf(stderr, "Arguments goes: [producers] [consumers] [buff_size] [file_name] [min_line_length] [search_mode] [info_mode] [exec_max_time]");
        exit(EXIT_FAILURE);
     }


    init(argv);

    start_threads(); // and wait for them

    finito_bonito();

    return 0;
}