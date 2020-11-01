#include "rw_lock.h"

#ifndef PRINT_FUNC_CALL
// #define PRINT_FUNC_CALL
#endif

void init_rwlock(struct rw_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  *rw = (struct rw_lock) {
    .num_readers = 0,
    .num_writers = 0,
    .cond_read = PTHREAD_COND_INITIALIZER,
    .cond_write = PTHREAD_COND_INITIALIZER,
    .cmutex_read = PTHREAD_MUTEX_INITIALIZER,
    .cmutex_write = PTHREAD_MUTEX_INITIALIZER,
    .mutex_read = PTHREAD_MUTEX_INITIALIZER,
    .mutex_write = PTHREAD_MUTEX_INITIALIZER,
  };
}

void r_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
#ifdef PRINT_FUNC_CALL
  printf("r_lock called\n");
#endif
  pthread_mutex_lock(&rw->cmutex_read);
  rw->num_readers ++;
  pthread_mutex_unlock(&rw->cmutex_read);

  pthread_mutex_lock(&rw->cmutex_write);
  while (rw->num_writers > 0) { // writer 빠질때까지 기다림.
    pthread_cond_wait(&rw->cond_write, &rw->cmutex_write);
  }
  pthread_mutex_unlock(&rw->cmutex_write);
}

void r_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
#ifdef PRINT_FUNC_CALL
  printf("r_unlock called\n");
#endif
  pthread_mutex_lock(&rw->cmutex_read);
  if (rw->num_readers > 0)
    rw->num_readers --;
  
  if (rw->num_readers == 0)
    pthread_cond_broadcast(&rw->cond_read);
  
  pthread_mutex_unlock(&rw->cmutex_read);
}

void w_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
#ifdef PRINT_FUNC_CALL
  printf("w_lock called\n");
#endif
  pthread_mutex_lock(&rw->cmutex_read);
  while (rw->num_readers > 0) {
    pthread_cond_wait(&rw->cond_read, &rw->cmutex_read);
  }
  pthread_mutex_unlock(&rw->cmutex_read);

  pthread_mutex_lock(&rw->cmutex_write);
  rw->num_writers ++;
  pthread_mutex_unlock(&rw->cmutex_write);

  pthread_mutex_lock(&rw->mutex_write);
}

void w_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
#ifdef PRINT_FUNC_CALL
  printf("w_unlock called\n");
#endif
  pthread_mutex_lock(&rw->cmutex_write);
  if (rw->num_writers > 0)
    rw->num_writers --;
  
  if (rw->num_writers == 0)
    pthread_cond_broadcast(&rw->cond_write);
  pthread_mutex_unlock(&rw->cmutex_write);

  pthread_mutex_unlock(&rw->mutex_write);
}
