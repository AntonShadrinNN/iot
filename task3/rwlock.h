#ifndef MY_RWLOCK_H
#define MY_RWLOCK_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t readers_cond;
    pthread_cond_t writers_cond;
    int readers_count;
    int waiting_writers;
    int writing;
} my_rwlock_t;

int my_rwlock_init(my_rwlock_t *rwlock);
int my_rwlock_destroy(my_rwlock_t *rwlock);
int my_rwlock_rdlock(my_rwlock_t *rwlock);
int my_rwlock_wrlock(my_rwlock_t *rwlock);
int my_rwlock_unlock(my_rwlock_t *rwlock);

#endif