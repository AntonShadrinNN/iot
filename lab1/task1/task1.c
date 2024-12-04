#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "timer.h"

typedef struct {
    long long trials_per_thread;
    long long hits_in_circle;
} ThreadData;

void* monte_carlo(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    long long hits = 0;
    unsigned int seed = time(NULL);
    for (long long i = 0; i < data->trials_per_thread; ++i) {
        double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;

        if (x * x + y * y <= 1.0) {
            hits++;
        }
    }

    data->hits_in_circle = hits;
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s nthreads ntrials\n", argv[0]);
        return 1;
    }

    int nthreads = atoi(argv[1]);
    long long ntrials = atoll(argv[2]);

    if (nthreads <= 0 || ntrials <= 0) {
        printf("Both nthreads and ntrials must be positive integers.\n");
        return 1;
    }

    pthread_t* threads = malloc(nthreads * sizeof(pthread_t));
    ThreadData* thread_data = malloc(nthreads * sizeof(ThreadData));

    long long trials_per_thread = ntrials / nthreads;

    double start_time, end_time;
    GET_TIME(start_time);

    for (int i = 0; i < nthreads; ++i) {
        thread_data[i].trials_per_thread = trials_per_thread;
        pthread_create(&threads[i], NULL, monte_carlo, &thread_data[i]);
    }

    long long total_hits = 0;
    for (int i = 0; i < nthreads; ++i) {
        pthread_join(threads[i], NULL);
        total_hits += thread_data[i].hits_in_circle;
    }

    double pi_estimate = 4.0 * (double)total_hits / (double)ntrials;

    GET_TIME(end_time);

    printf("Estimated value of Pi: %f\n", pi_estimate);
    printf("Elapsed time: %f seconds\n", end_time - start_time);
    free(threads);
    free(thread_data);
    return 0;
}
