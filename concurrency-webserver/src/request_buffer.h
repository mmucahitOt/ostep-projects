#ifndef REQUEST_BUFFER_H
#define REQUEST_BUFFER_H

typedef struct {
    int fd;       /* connection socket from accept */
    int filesize; /* -1 = unknown; SFF will use this later */
} request_t;

typedef enum {
    SCHED_FIFO = 0,
    SCHED_SFF  = 1
} sched_policy_t;

/* capacity >= 1 */
void buffer_init(int capacity, sched_policy_t policy);
void buffer_destroy(void);

/* Phase 1: NO locks. Caller must ensure single-threaded use.
 * produce: fails (returns -1) if full
 * consume: fails (returns -1) if empty; on success fills *out
 */
int buffer_produce(request_t r);
int buffer_consume(request_t *out);

int buffer_count(void);
int buffer_capacity(void);

#endif