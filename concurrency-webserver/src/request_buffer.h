#ifndef REQUEST_BUFFER_H
#define REQUEST_BUFFER_H

typedef struct {
    int fd;
    int filesize; /* -1 = unknown; SFF later */
} request_t;

typedef enum {
    SCHED_FIFO = 0,
    SCHED_SFF  = 1
} sched_policy_t;

void buffer_init(int capacity, sched_policy_t policy);
void buffer_destroy(void);

/* Thread-safe. Blocks if full / empty. */
void buffer_produce(request_t r);
void buffer_consume(request_t *out);

/* Safe to call; takes the lock briefly. */
int buffer_count(void);
int buffer_capacity(void);

#endif