#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <math.h>
#include <pthread.h>

int k; // filter matrix size
int X; // x
int Y; // y
int M; // color max

typedef struct threaddata{
    int ** image_matrix;
    double ** filter_matrix;
    int ** filtered_matrix;
    int start;
    int stop;
}threaddata;

void print_time(int threads_nr, struct timespec start, struct timespec end) {
    unsigned long long ms = (end.tv_sec - start.tv_sec) * 1000000;
    ms += (end.tv_nsec - start.tv_nsec) / 1000;
    printf("Threads: %d, Filter: %d, Size: %dx%d, Time [ms]: %llu\n", threads_nr, k, X, Y, ms);
}

double ** read_filter_matrix(char * file_name){
    FILE * file = fopen(file_name,"r");
    int licznik, mianownik, tmp;
    double ** filter_matrix = NULL;
    double wsp;

    fscanf(file,"%d\n%d %d\n",&k,&licznik, &mianownik);
    wsp = licznik/(double)mianownik;


    filter_matrix = malloc(k * sizeof(double*));
    for(int i = 0; i < k; i++){
        filter_matrix[i] = malloc(k * sizeof(double));
        for(int j = 0; j < k; j++){
            fscanf(file,"%d",&tmp);
            filter_matrix[i][j] = tmp * wsp;
        }
    }
    fclose(file);
    return filter_matrix;
}

int ** read_image_matrix(char * file_name){
    FILE * file;
    if((file = fopen(file_name,"r")) == NULL)
    {
        perror(file_name);
        exit(EXIT_FAILURE);
    }
    int ** image_matrix = NULL;
    int buffer_size;


    char line[100]; fgets(line,100,file); fgets(line,100,file);
    if(line[0] == '#') fscanf(file,"%d %d\n", &X, &Y);
    else sscanf(line,"%d %d\n",&X, &Y);
    fscanf(file,"%d\n",&M);

    image_matrix = malloc(Y * sizeof(int*));
    for(int y = 0; y < Y; y++){
        image_matrix[y] = malloc(X * sizeof(int));
        for(int x = 0; x < X; x++){
             fscanf(file,"%d",&image_matrix[y][x]);
        }
    }

    fclose(file);
    return image_matrix;
}

void save_to_file(char * filename, int ** new_image){
    FILE * file = fopen(filename,"w+");
    fprintf(file, "P2\n");
    fprintf(file, "# %s\n",filename);
    fprintf(file, "%d %d\n", X, Y);
    fprintf(file, "%d\n", 255);
    for(int y = 0; y < Y; y++){
        for(int x = 0; x < X; x++){
            fprintf(file,"%d ", new_image[y][x]);
        }
        fprintf(file,"\n");
    }
    fclose(file);
}

int calculate_pixel(int x, int y, double ** filter_matrix, int ** image_matrix){
    int mid = (int) (k/(double)2);
    double summ = 0;
    int ay, bx;
    for(int i = 0; i < k; i++){
        ay = (int) fmin(fmax(0,y-mid+i),Y-1);
        for(int j = 0; j < k; j++){
            bx = (int) fmin(fmax(0,x-mid+j),X-1);
            summ += image_matrix[ay][bx] * filter_matrix[i][j];
        }
    }
    return (int) round(fmin(fmax(summ, 0),255));
}

void filter_image(int ** image_matrix, double ** filter_matrix, int ** filtered_matrix, int start, int stop){
    for(int y = start; y < stop; y++)
        for(int x = 0; x < X; x++)
            filtered_matrix[y][x] = calculate_pixel(x, y, filter_matrix, image_matrix);
}

void free_matrixes(double ** filter_matrix, int ** image_matrix, int ** filtered_image_matrix){
    for(int i = 0; i < k; i++)
        free(filter_matrix[i]);
    free(filter_matrix);

    for(int y = 0; y < Y; y++)
        free(image_matrix[y]);
    free(image_matrix);

    for(int y = 0; y < Y; y++)
        free(filtered_image_matrix[y]);
    free(filtered_image_matrix);
}


void * thread_start(threaddata * args){
    filter_image(args->image_matrix,args->filter_matrix,args->filtered_matrix,args->start,args->stop);
}

int ** calc_using_threads(int thread_quantity, double ** filter_matrix, int ** image_matrix){
    pthread_t threads[thread_quantity];
    threaddata data[thread_quantity];
    pthread_attr_t* attr = calloc(1, sizeof(pthread_attr_t));
    int step = (int) ceil(Y/(double)thread_quantity);

    int ** filtered_image_matrix = malloc(Y * sizeof(int*));
    for(int i = 0; i < Y; i++)
        filtered_image_matrix[i] = malloc(X * sizeof(int));


    struct timespec start_time, end_time;
    clock_gettime(CLOCK_REALTIME, &start_time);


    for(int i = 0; i < thread_quantity; i++){
        data[i].filter_matrix = filter_matrix;
        data[i].image_matrix = image_matrix;
        data[i].filtered_matrix = filtered_image_matrix;
        data[i].start = i * step;
        data[i].stop = (int) fmin(data[i].start+step,Y);

        pthread_attr_init(attr);
        pthread_create(&threads[i],attr,(void *)thread_start,(void *)&data[i]);

        pthread_attr_destroy(attr);
    }

    for(int i = 0; i < thread_quantity; i++)
        pthread_join(threads[i], NULL);
    clock_gettime(CLOCK_REALTIME, &end_time);
    print_time(thread_quantity, start_time, end_time);

    free(attr);
    return filtered_image_matrix;
}

int main(int argc, char *argv[]){
    if(argc < 4){
        printf("Too less arguments. Write: [threads] [filter_path] [image_path] [output_path]");
        exit(EXIT_FAILURE);
    }
    int threads = atoi(argv[1]);
    double ** filter_matrix = read_filter_matrix(argv[2]);
    int ** image_matrix = read_image_matrix(argv[3]);
    int ** filtered_image_matrix = calc_using_threads(threads,filter_matrix,image_matrix);
    save_to_file(argv[4],filtered_image_matrix);

    free_matrixes(filter_matrix, image_matrix, filtered_image_matrix);
    return 0;
}