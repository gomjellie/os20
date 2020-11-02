#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <unistd.h>
#include "SSU_Sem.h"

#ifndef PRINT_FUNC_CALL
// #define PRINT_FUNC_CALL
#endif

void SSU_Sem_init(SSU_Sem *s, int value) {
  *s = (SSU_Sem) {
    .counter = value,
    .signal = PTHREAD_COND_INITIALIZER,
    .mutex_counter = PTHREAD_MUTEX_INITIALIZER,
    .mutex_up = PTHREAD_MUTEX_INITIALIZER,
    .mutex_down = PTHREAD_MUTEX_INITIALIZER,
  };
}

void SSU_Sem_down(SSU_Sem *s) {
  pthread_mutex_lock(&s->mutex_down);
#ifdef PRINT_FUNC_CALL
  printf("SSU_Sem_down entered id: %d\n", (int)pthread_self());
#endif
  pthread_mutex_lock(&s->mutex_counter);

  while (s->counter <= 0) {
    // printf("counter: %d\n", s->counter);
    pthread_cond_wait(&s->signal, &s->mutex_counter);
  }
  s->counter --;
  usleep(500);
  
  pthread_mutex_unlock(&s->mutex_counter);
  
#ifdef PRINT_FUNC_CALL
  printf("SSU_Sem_down exit id: %d\n", (int)pthread_self());
#endif
  pthread_mutex_unlock(&s->mutex_down);
}

void SSU_Sem_up(SSU_Sem *s) {
  pthread_mutex_lock(&s->mutex_up);
#ifdef PRINT_FUNC_CALL
  printf("SSU_Sem_up entered id: %d\n", (int)pthread_self());
#endif
  pthread_mutex_lock(&s->mutex_counter);
  s->counter ++;
  usleep(500);

  pthread_cond_broadcast(&s->signal);
  pthread_mutex_unlock(&s->mutex_counter);
#ifdef PRINT_FUNC_CALL
  printf("SSU_Sem_up exit %d\n", (int)pthread_self());
#endif
  pthread_mutex_unlock(&s->mutex_up);
}
