#include "sleeplock.h"
#include "semaphore.h"

struct sem_buffer {
    int value;
};

#define NUM_SEM_BUFFER  20
extern struct sem_buffer sem_buf_arr[NUM_SEM_BUFFER];

extern struct sem_t pro;
extern struct sem_t con;
extern struct sem_t empty;
extern struct sem_t full;
