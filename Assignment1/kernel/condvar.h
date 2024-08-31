#ifndef COND
#define COND
#include "sleeplock.h"
typedef int cond_t;

void cond_wait (cond_t *cv, struct sleeplock *lock);
void cond_signal (cond_t *cv);
void cond_broadcast (cond_t *cv);
#endif