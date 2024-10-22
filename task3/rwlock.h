#ifndef MY_RWLOCK_H
#define MY_RWLOCK_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;           // Мьютекс для защиты структуры
    pthread_cond_t readers_cond;     // Условная переменная для читателей
    pthread_cond_t writers_cond;     // Условная переменная для писателей
    int readers_count;                // Счетчик читателей
    int waiting_writers;              // Количество ожидающих писателей
    int writing;                      // Флаг, указывающий, что идет запись
} my_rwlock_t;

int my_rwlock_init(my_rwlock_t *rwlock);
int my_rwlock_destroy(my_rwlock_t *rwlock);
int my_rwlock_rdlock(my_rwlock_t *rwlock);
int my_rwlock_wrlock(my_rwlock_t *rwlock);
int my_rwlock_unlock(my_rwlock_t *rwlock);

#endif // MY_RWLOCK_H
