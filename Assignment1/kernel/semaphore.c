#include "semaphore.h"

void sem_init(struct sem_t *s, int value){
    s->value = value;
    
    s->cv = 0;
    
    initsleeplock(&s->lock,"sem-lock");

}

void sem_wait(struct sem_t *s){
    acquiresleep(&s->lock);
    while(s->value == 0) cond_wait(&s->cv,&s->lock);

    s->value--;

    releasesleep(&s->lock);
}

void sem_post(struct sem_t *s){
    acquiresleep(&s->lock);

    s->value++;

    cond_signal(&s->cv);

    releasesleep(&s->lock);
}


