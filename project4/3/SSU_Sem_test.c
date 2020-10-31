#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "SSU_Sem.h"

SSU_Sem child;
SSU_Sem parent;

void *justprint(void *data)
{
  int thread_id = *((int *)data);
  SSU_Sem_down(&child);
  printf("This is thread %d\n", thread_id);
  SSU_Sem_up(&parent);
  return 0;
}

int main(int argc, char *argv[])
{

  pthread_t t1, t2;
  int t1id = 1, t2id = 2;

  SSU_Sem_init(&child, 0);
  SSU_Sem_init(&parent, 0);
  
  pthread_create(&t1, NULL, justprint, &t1id);
  pthread_create(&t2, NULL, justprint, &t2id);
  
  sleep(1);


  //in spite of sleep, this should print first
  printf("This is main thread. This should print first\n");

  SSU_Sem_up(&child);
  SSU_Sem_down(&parent);
  printf("One thread has printed\n");
  
  SSU_Sem_up(&child);
  SSU_Sem_down(&parent);
  printf("Second thread has printed\n");
  
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  
  return 0;
}
