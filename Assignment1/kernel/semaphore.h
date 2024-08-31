#ifndef SEM
#define SEM

#include "sleeplock.h"
#include "condvar.h"

struct sem_t{
    int value;
    cond_t cv;
    struct sleeplock lock;
};

void sem_init(struct sem_t *s, int value);
void sem_wait(struct sem_t *s);
void sem_post(struct sem_t *s);

#endif