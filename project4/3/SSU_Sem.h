#include <pthread.h>

typedef struct SSU_Sem {
    int counter;

    pthread_cond_t signal;

    pthread_mutex_t mutex_counter;
    pthread_mutex_t mutex_up;
    pthread_mutex_t mutex_down;
} SSU_Sem;

void SSU_Sem_init(SSU_Sem *, int);
void SSU_Sem_up(SSU_Sem *);
void SSU_Sem_down(SSU_Sem *);
