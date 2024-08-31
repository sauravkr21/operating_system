struct barr {
    cond_t cv;
    struct sleeplock lock;
    int set;
    int processes;
};

#define NUM_BARRIER     10
extern struct barr barriers[NUM_BARRIER];