#include "rwlock.h"

// Инициализация rwlock
int my_rwlock_init(my_rwlock_t *rwlock) {
    if (pthread_mutex_init(&rwlock->mutex, NULL) != 0) {
        return -1;
    }
    if (pthread_cond_init(&rwlock->readers_cond, NULL) != 0) {
        pthread_mutex_destroy(&rwlock->mutex);
        return -1;
    }
    if (pthread_cond_init(&rwlock->writers_cond, NULL) != 0) {
        pthread_cond_destroy(&rwlock->readers_cond);
        pthread_mutex_destroy(&rwlock->mutex);
        return -1;
    }
    rwlock->readers_count = 0;
    rwlock->waiting_writers = 0;
    rwlock->writing = 0;
    return 0;
}

// Уничтожение rwlock
int my_rwlock_destroy(my_rwlock_t *rwlock) {
    pthread_mutex_destroy(&rwlock->mutex);
    pthread_cond_destroy(&rwlock->readers_cond);
    pthread_cond_destroy(&rwlock->writers_cond);
    return 0;
}

// Функция блокировки для чтения
int my_rwlock_rdlock(my_rwlock_t *rwlock) {
    pthread_mutex_lock(&rwlock->mutex);
    while (rwlock->waiting_writers > 0 || rwlock->writing) {
        pthread_cond_wait(&rwlock->readers_cond, &rwlock->mutex);
    }
    rwlock->readers_count++;
    pthread_mutex_unlock(&rwlock->mutex);
    return 0;
}

// Функция блокировки для записи
int my_rwlock_wrlock(my_rwlock_t *rwlock) {
    pthread_mutex_lock(&rwlock->mutex);
    rwlock->waiting_writers++;
    while (rwlock->readers_count > 0 || rwlock->writing) {
        pthread_cond_wait(&rwlock->writers_cond, &rwlock->mutex);
    }
    rwlock->waiting_writers--;
    rwlock->writing = 1;
    pthread_mutex_unlock(&rwlock->mutex);
    return 0;
}

// Функция разблокировки
int my_rwlock_unlock(my_rwlock_t *rwlock) {
    pthread_mutex_lock(&rwlock->mutex);
    if (rwlock->writing) {
        rwlock->writing = 0;
        pthread_cond_broadcast(&rwlock->readers_cond);
        pthread_cond_signal(&rwlock->writers_cond);
    } else {
        rwlock->readers_count--;
        if (rwlock->readers_count == 0 && rwlock->waiting_writers > 0) {
            pthread_cond_signal(&rwlock->writers_cond);
        }
    }
    pthread_mutex_unlock(&rwlock->mutex);
    return 0;
}
