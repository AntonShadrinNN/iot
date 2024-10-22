#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <complex.h>
#include <math.h>
#include "timer.h"

#define MAX_ITER 1000
#define X_MIN -2.0
#define X_MAX 1.0
#define Y_MIN -1.5
#define Y_MAX 1.5

pthread_mutex_t file_mutex;

typedef struct {
    int thread_id;
    int nthreads;
    int npoints;
    FILE *file;
} thread_data_t;

int mandelbrot(double complex c) {
    double complex z = 0;
    int iter;
    for (iter = 0; iter < MAX_ITER; iter++) {
        z = z*z + c;
        if (cabs(z) > 2.0)
            return 0;
    }
    return 1;
}

void *generate_points(void *arg) {
    thread_data_t *data = (thread_data_t*)arg;
    int start = (data->npoints / data->nthreads) * data->thread_id;
    int end = (data->thread_id == data->nthreads - 1) ? data->npoints : start + (data->npoints / data->nthreads);
    double dx = (X_MAX - X_MIN) / sqrt(data->npoints);
    double dy = (Y_MAX - Y_MIN) / sqrt(data->npoints);

    for (int i = start; i < end; i++) {
        for (int j = 0; j < sqrt(data->npoints); j++) {
            double x = X_MIN + i * dx;
            double y = Y_MIN + j * dy;
            double complex c = x + y * I;
            if (mandelbrot(c)) {
                pthread_mutex_lock(&file_mutex);
                fprintf(data->file, "%lf,%lf\n", x, y);
                pthread_mutex_unlock(&file_mutex);
            }
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./program nthreads npoints\n");
        return 1;
    }

    int nthreads = atoi(argv[1]);
    int npoints = atoi(argv[2]);

    FILE *file = fopen("mandelbrot.csv", "w");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    pthread_mutex_init(&file_mutex, NULL);

    pthread_t threads[nthreads];
    thread_data_t thread_data[nthreads];

    double start_time, end_time;
    GET_TIME(start_time);

    for (int i = 0; i < nthreads; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].nthreads = nthreads;
        thread_data[i].npoints = npoints;
        thread_data[i].file = file;

        if (pthread_create(&threads[i], NULL, generate_points, &thread_data[i]) != 0) {
            perror("Error creating thread");
            return 1;
        }
    }

    for (int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(file);
    pthread_mutex_destroy(&file_mutex);

    GET_TIME(end_time);

    printf("Elapsed time: %f seconds\n", end_time - start_time);
    printf("Finished generating mandelbrot.csv\n");
    return 0;
}
