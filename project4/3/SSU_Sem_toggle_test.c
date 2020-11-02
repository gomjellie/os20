#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <unistd.h>
#include "SSU_Sem.h"

#define NUM_THREADS 3
#define NUM_ITER 10

SSU_Sem semaphore[NUM_THREADS];

void *justprint(void *data)
{
  int thread_id = *((int *)data);

  for(int i=0; i < NUM_ITER; i++)
    {
      SSU_Sem_down(&semaphore[thread_id]);
      printf("This is thread %d\n", thread_id);
    }
  return 0;
}

int main(int argc, char *argv[])
{

  pthread_t mythreads[NUM_THREADS];
  int mythread_id[NUM_THREADS];
  
  for (int i = 0; i < NUM_THREADS; i++)
    SSU_Sem_init(&semaphore[i], 0);
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      mythread_id[i] = i;
      pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
      usleep(500);
    }
  
  for (int j = 0; j < NUM_ITER; j++) {
    for (int i = 0; i < NUM_THREADS; i++) {
      SSU_Sem_up(&semaphore[i]);
    }
  }
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      pthread_join(mythreads[i], NULL);
    }
  
  return 0;
}
