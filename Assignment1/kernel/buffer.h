struct buffer {
    struct sleeplock lock;
    int value;
    int full;
    cond_t inserted;
    cond_t deleted;
};

#define NUM_BUFFER  20
extern struct buffer buf_arr[NUM_BUFFER];
extern struct sleeplock insert_lock;
extern struct sleeplock delete_lock;