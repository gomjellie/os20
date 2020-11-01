#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/param.h>

int item_to_produce, curr_buf_size;
int total_items, max_buf_size, num_workers, num_masters;

int *buffer;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_can_consume = PTHREAD_COND_INITIALIZER; // 소비할것이 생김.
pthread_cond_t cond_can_produce = PTHREAD_COND_INITIALIZER; // buffer 공간있으면 signal

void print_produced(int num, int master) {
  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {
  printf("Consumed %d by worker %d\n", num, worker);
}

//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);
  int num_chunks = total_items / num_masters;
  int start = thread_id * num_chunks;
  int end = MIN((thread_id + 1) * num_chunks, total_items);

  if (thread_id == num_masters - 1) end += total_items % num_masters;

  for (int idx = start; idx < end; idx++) {
    pthread_mutex_lock(&mutex);
    if (curr_buf_size == max_buf_size) {
      pthread_cond_signal(&cond_can_consume);
      if (item_to_produce != total_items)
        pthread_cond_wait(&cond_can_produce, &mutex); // 자리 날때까지 기다림
    }
    buffer[curr_buf_size++] = idx;
    print_produced(idx, thread_id);
    item_to_produce++;
    pthread_mutex_unlock(&mutex);
  }
  pthread_mutex_lock(&mutex);
  pthread_cond_signal(&cond_can_consume);
  pthread_mutex_unlock(&mutex);

  return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item
void *generate_responses_loop(void *data)
{
  int thread_id = *((int *)data);
  int num_chunks = total_items / num_workers;
  int start = thread_id * num_chunks;
  int end = MIN((thread_id + 1) * num_chunks, total_items);

  if (thread_id == 0) end += total_items % num_workers;

  for (int item_to_consume = start; item_to_consume < end; item_to_consume++) {
    pthread_mutex_lock(&mutex);
    if (curr_buf_size == 0) {
      pthread_cond_signal(&cond_can_produce);
      pthread_cond_wait(&cond_can_consume, &mutex);
    }
    int consumed = buffer[--curr_buf_size];
    buffer[curr_buf_size] = 0;
    print_consumed(consumed, thread_id);
    pthread_mutex_unlock(&mutex);
  }
  pthread_mutex_lock(&mutex);
  pthread_cond_signal(&cond_can_produce);
  pthread_mutex_unlock(&mutex);
  
  return 0;
}

int main(int argc, char *argv[])
{
  int *master_thread_ids, *worker_thread_ids;
  pthread_t *master_threads, *worker_threads;
  item_to_produce = 0;
  curr_buf_size = 0;
  
  int i;
  
  if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }

  buffer = (int *)malloc (sizeof(int) * max_buf_size);

   //create master producer threads
  master_thread_ids = (int *)malloc(sizeof(int) * num_masters);
  master_threads = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
  for (i = 0; i < num_masters; i++)
    master_thread_ids[i] = i;

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_threads[i], NULL, generate_requests_loop, (void *)&master_thread_ids[i]);
  
  // pthread_mutex_lock(&mutex);
  // pthread_cond_wait(&cond_can_consume, &mutex);
  // pthread_mutex_unlock(&mutex);

  //create worker consumer threads
  worker_thread_ids = malloc(sizeof(int) * num_workers);
  worker_threads = malloc(sizeof(pthread_t) * num_workers);
  for (i = 0; i < num_workers; i++)
    worker_thread_ids[i] = i;
  
  for (i = 0; i < num_workers; i++)
    pthread_create(&worker_threads[i], NULL, generate_responses_loop, (void *)&worker_thread_ids[i]);

  //wait for all threads to complete
  for (i = 0; i < num_masters; i++) {
    pthread_join(master_threads[i], NULL);
    printf("master %d joined\n", i);
    pthread_cond_broadcast(&cond_can_consume);
  }

  for (i = 0; i < num_workers; i++) {
    pthread_join(worker_threads[i], NULL);
    printf("worker %d joined\n", i);
    pthread_cond_broadcast(&cond_can_produce);
  }
  
  /*----Deallocating Buffers---------------------*/
  pthread_mutex_destroy(&mutex);
  free(buffer);
  free(master_thread_ids);
  free(master_threads);
  
  return 0;
}
