#include "rw_lock.h"
#include <time.h>

long indx;
long *readerAcquireTime;
long *readerReleaseTime;
long *writerAcquireTime;
long *writerReleaseTime;

struct rw_lock rwlock;
pthread_spinlock_t spinlock;

void *Reader(void* arg)
{
	int threadNUmber = *((int *)arg);

	// Occupying the Lock
	r_lock(&rwlock);

	pthread_spin_lock(&spinlock);
	readerAcquireTime[threadNUmber] = indx;
	indx++;
	pthread_spin_unlock(&spinlock);

	// printf("Reader: %d has acquired the lock\n", threadNUmber);
	usleep(10000);

	pthread_spin_lock(&spinlock);
	readerReleaseTime[threadNUmber] = indx;
	indx++;
	pthread_spin_unlock(&spinlock);

	// Releasing the Lock
	r_unlock(&rwlock);
	// printf("Reader: %d has released the lock\n",threadNUmber);
}

void *Writer(void* arg)
{
	int threadNUmber = *((int *)arg);

  // Occupying the Lock
	w_lock(&rwlock);

	pthread_spin_lock(&spinlock);
	writerAcquireTime[threadNUmber] = indx;
	indx++;
	pthread_spin_unlock(&spinlock);

	// printf("Writer: %d has acquired the lock\n",threadNUmber);
  usleep(10000);

	pthread_spin_lock(&spinlock);
	writerReleaseTime[threadNUmber] = indx;
	indx++;
	pthread_spin_unlock(&spinlock);

	// Releasing the Lock
	w_unlock(&rwlock);
	// printf("Writer: %d has released the lock\n",threadNUmber);
}

int main(int argc, char *argv[])
{

	int *threadNUmber;
	pthread_t *threads;

	init_rwlock(&rwlock);
	pthread_spin_init(&spinlock, 0);

	int read_num_threads;
	int write_num_threads;

	read_num_threads = atoi(argv[1]);
	write_num_threads = atoi(argv[2]);

	indx = 0;
	readerAcquireTime = malloc(read_num_threads*2*sizeof(long));
	readerReleaseTime = malloc(read_num_threads*2*sizeof(long));
	writerAcquireTime = malloc(write_num_threads*sizeof(long));
	writerReleaseTime = malloc(write_num_threads*sizeof(long));

	int num_threads = 2*read_num_threads + write_num_threads;

	threads = (pthread_t*) malloc(num_threads * (sizeof(pthread_t)));

	int count = 0;
	for(int i=0; i<read_num_threads; i++)
	{
		int *arg = (int *)malloc((sizeof(int)));
		if (arg == NULL){
			printf("Couldn't allocate memory for thread arg.\n");
			exit(EXIT_FAILURE);
		}
		*arg = i;
		int ret = pthread_create(threads+count, NULL, Reader, (void*) arg);
		if(ret){
        printf("Error - pthread_create() return code: %d\n",ret);
        exit(EXIT_FAILURE);
    }
		count++;
	}

	for(int i=0;i<write_num_threads;i++)
	{
		int *arg = (int *)malloc((sizeof(int)));
		if (arg == NULL){
			printf("Couldn't allocate memory for thread arg.\n");
			exit(EXIT_FAILURE);
		}
		*arg = i;
		int ret = pthread_create(threads+count, NULL, Writer,( void*) arg);
		if(ret)
    {
        printf("Error - pthread_create() return code: %d\n",ret);
        exit(EXIT_FAILURE);
    }
		count++;
	}

	for(int i=0;i<read_num_threads;i++)
	{
		int *arg = (int *)malloc((sizeof(int)));
		if (arg == NULL)
		{
			printf("Couldn't allocate memory for thread arg.\n");
			exit(EXIT_FAILURE);
		}
		*arg = read_num_threads + i;
		int ret = pthread_create(threads+count,NULL,Reader,(void*) arg);
		if(ret)
    {
        printf("Error - pthread_create() return code: %d\n",ret);
        exit(EXIT_FAILURE);
    }
		count++;
	}


	for(int i=0;i<num_threads; i++)
		pthread_join(threads[i],NULL);

	// for(int i=0; i<read_num_threads*2; i++)
	// 	printf("Reader %d Lock Time: %ld Unlock Time: %ld\n", i, readerAcquireTime[i], readerReleaseTime[i]);

	// for (int i = 0; i < write_num_threads; i++)
	// 	printf("Writer %d Lock Time: %ld Unlock Time: %ld\n", i, writerAcquireTime[i], writerReleaseTime[i]);

	long *max_reader_acquire_time = max_element(readerAcquireTime, readerAcquireTime + 2 * read_num_threads);
	long *min_reader_release_time = min_element(readerReleaseTime, readerReleaseTime + 2 * read_num_threads);
	long *max_reader_release_time = max_element(readerReleaseTime, readerReleaseTime + 2 * read_num_threads);
	long *min_writer_acquire_time = min_element(writerAcquireTime, writerAcquireTime + write_num_threads);

	// check if all readers get lock immediately
	if ((read_num_threads > 0) && (*max_reader_acquire_time > *min_reader_release_time)){
		printf("Reader should not wait to acquire lock\n");
		exit(0);
	}

	// All readers get lock before any writer
	if ((read_num_threads > 0) && (write_num_threads > 0) && (*min_writer_acquire_time < *max_reader_release_time)){
		printf("All readers get lock before any writer\n");
		exit(0);
	}

	// check if writer exited immediately
	for (int i = 0; i < write_num_threads; i++)
		if ((writerReleaseTime[i] - writerAcquireTime[i]) != 1){
			printf("No reader/ writer is allowed when a writer holds lock\n");
			exit(0);
		}

	// printf("%ld\n",*max_reader_acquire_time);
	// printf("%ld\n",*min_reader_release_time);
	// printf("%ld\n",*max_reader_release_time);
	// printf("%ld\n",*min_writer_acquire_time);
	printf("PASSED\n");

	free(readerAcquireTime);
	free(readerReleaseTime);
	free(writerAcquireTime);
	free(writerReleaseTime);
}
