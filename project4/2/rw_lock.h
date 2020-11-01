#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct rw_lock
{
    long num_readers;
    long num_writers;

    pthread_cond_t cond_write;
    pthread_cond_t cond_read;

    pthread_mutex_t cmutex_write; // cond mutex
    pthread_mutex_t cmutex_read;

    pthread_mutex_t mutex_write;
    pthread_mutex_t mutex_read;
};

void init_rwlock(struct rw_lock * rw);
void r_lock(struct rw_lock * rw);
void r_unlock(struct rw_lock * rw);
void w_lock(struct rw_lock * rw);
void w_unlock(struct rw_lock * rw);
long *max_element(long* start, long* end);
long *min_element(long* start, long* end);
