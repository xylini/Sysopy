#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>


int producers;
int consumers;
int buff_size;
int min_line_length;
int max_work_time;
int search_mode;
int info_mode;

char **buffer;
pthread_mutex_t *buff_mutex;
pthread_mutex_t prod_mutex;
pthread_mutex_t cons_mutex;

pthread_cond_t w_cond;
pthread_cond_t r_cond;
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
    buff_mutex = malloc((buff_size) * sizeof(pthread_mutex_t));

    for (int i = 0; i < buff_size; ++i){
        pthread_mutex_init(&buff_mutex[i], NULL);
        buffer[i] = NULL;
    }

    pthread_mutex_init(&prod_mutex, NULL);
    pthread_mutex_init(&cons_mutex, NULL);
    pthread_cond_init(&w_cond, NULL);
    pthread_cond_init(&r_cond, NULL);
}


void finito_bonito() {
    if (file) fclose(file);

    for (int i = 0; i < buff_size; i++){
        if (buffer[i]) free(buffer[i]);
        pthread_mutex_destroy(&buff_mutex[i]);

    }
    free(buffer);
    free(buff_mutex);

    pthread_mutex_destroy(&prod_mutex);
    pthread_mutex_destroy(&cons_mutex);

    pthread_cond_destroy(&w_cond);
    pthread_cond_destroy(&r_cond);
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
        pthread_mutex_lock(&prod_mutex);

        while (buffer[last_prod_index] != NULL)
            pthread_cond_wait(&w_cond, &prod_mutex);

        index = last_prod_index;
        if(info_mode) fprintf(stderr, "P: %ld reading buff index (%d).\n", (long) pthread_self(), index);
        last_prod_index = (last_prod_index + 1) % buff_size;


        pthread_mutex_lock(&buff_mutex[index]);

        buffer[index] = malloc((strlen(line) + 1) * sizeof(char));
        strcpy(buffer[index], line);
        if(info_mode) fprintf(stderr, "P: %ld line copied to buffer at index (%d).\n", (long) pthread_self(), index);

        pthread_cond_broadcast(&r_cond);
        pthread_mutex_unlock(&buff_mutex[index]);
        pthread_mutex_unlock(&prod_mutex);
    }
    if(info_mode) fprintf(stderr, "P: %ld the job is done.\n", (long) pthread_self());
    return NULL;
}

void *consumer(void *pVoid) {
    char *line;
    int index;
    while (1) {
        pthread_mutex_lock(&cons_mutex);

        while (buffer[last_cons_index] == NULL) {
            if (is_writing_done) {
                pthread_mutex_unlock(&cons_mutex);
                if(info_mode) fprintf(stderr, "C: %ld the job is done. \n", (long) pthread_self());
                return NULL;
            }
            pthread_cond_wait(&r_cond, &cons_mutex);
        }

        index = last_cons_index;
        if(info_mode) fprintf(stderr, "C: %ld reading buff index (%d).\n", (long) pthread_self(), index);
        last_cons_index = (last_cons_index + 1) % buff_size;

        pthread_mutex_lock(&buff_mutex[index]);
        pthread_mutex_unlock(&cons_mutex);

        line = buffer[index];
        buffer[index] = NULL;
        if(info_mode) fprintf(stderr, "C: %ld taking line from buffer at index (%d).\n", (long) pthread_self(), index);

        pthread_cond_broadcast(&w_cond);
        pthread_mutex_unlock(&buff_mutex[index]);

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

    for (int p = 0; p < producers; ++p) pthread_join(prod_thread[p], NULL);

    is_writing_done = 1;
    pthread_cond_broadcast(&r_cond);

    for (int k = 0; k < consumers; ++k) pthread_join(cons_thread[k], NULL);
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