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
static pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

static int occupied_index(int k) {
    /* k = 0 .. count-1, oldest first */
    return (out_idx + k) % capacity;
}
static int pick_index(void) {
    if (policy == SCHED_FIFO)
        return out_idx;
    /* SFF: smallest filesize; unknown (-1) counts as "very large"
     * tie → older request (smaller k) so all -1 acts like FIFO
     */
    int best_k = 0;
    int best_size = slots[occupied_index(0)].filesize;
    if (best_size < 0)
        best_size = 0x7fffffff; /* INT_MAX */
    for (int k = 1; k < count; k++) {
        int sz = slots[occupied_index(k)].filesize;
        if (sz < 0)
            sz = 0x7fffffff;
        if (sz < best_size) {
            best_size = sz;
            best_k = k;
        }
    }
    return occupied_index(best_k);
}
static void remove_at(int idx, request_t *out) {
    request_t tmp = slots[idx];
    slots[idx] = slots[out_idx];
    slots[out_idx] = tmp;
    *out = slots[out_idx];
    out_idx = (out_idx + 1) % capacity;
    count--;
}

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
    remove_at(pick_index(), out);

    pthread_cond_signal(&not_full);
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