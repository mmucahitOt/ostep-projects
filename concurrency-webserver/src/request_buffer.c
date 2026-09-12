#include "request_buffer.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static request_t      *slots = NULL;
static int             capacity = 0;
static int             count = 0;
static int             in_idx = 0;
static int             out_idx = 0;
static sched_policy_t  policy = SCHED_FIFO;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  not_full  = PTHREAD_COND_INITIALIZER;
static pthread_cond_t  not_empty = PTHREAD_COND_INITIALIZER;

void buffer_init(int cap, sched_policy_t pol) {
    assert(cap >= 1);

    pthread_mutex_lock(&mutex);

    free(slots);
    slots = malloc(sizeof(request_t) * (size_t)cap);
    assert(slots != NULL);

    capacity = cap;
    count = 0;
    in_idx = 0;
    out_idx = 0;
    policy = pol;

    pthread_mutex_unlock(&mutex);
}

void buffer_destroy(void) {
    pthread_mutex_lock(&mutex);

    free(slots);
    slots = NULL;
    capacity = count = in_idx = out_idx = 0;

    pthread_mutex_unlock(&mutex);
}

void buffer_produce(request_t r) {
    pthread_mutex_lock(&mutex);

    while (count == capacity)
        pthread_cond_wait(&not_full, &mutex);

    assert(slots != NULL);
    slots[in_idx] = r;
    in_idx = (in_idx + 1) % capacity;
    count++;

    pthread_cond_signal(&not_empty);  /* wake one consumer */
    pthread_mutex_unlock(&mutex);
}

void buffer_consume(request_t *out) {
    assert(out != NULL);

    pthread_mutex_lock(&mutex);

    while (count == 0)
        pthread_cond_wait(&not_empty, &mutex);

    assert(slots != NULL);
    (void)policy; /* SFF later */

    *out = slots[out_idx];
    out_idx = (out_idx + 1) % capacity;
    count--;

    pthread_cond_signal(&not_full);   /* wake one producer */
    pthread_mutex_unlock(&mutex);
}

int buffer_count(void) {
    int c;
    pthread_mutex_lock(&mutex);
    c = count;
    pthread_mutex_unlock(&mutex);
    return c;
}

int buffer_capacity(void) {
    int c;
    pthread_mutex_lock(&mutex);
    c = capacity;
    pthread_mutex_unlock(&mutex);
    return c;
}