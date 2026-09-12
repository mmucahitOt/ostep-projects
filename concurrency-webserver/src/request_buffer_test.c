#include "request_buffer.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static request_t req(int fd) {
    request_t r = { .fd = fd, .filesize = -1 };
    return r;
}

/* --- Tests A–C: same FIFO logic, no full/empty -1 checks --- */

static void *producer_full(void *arg) {
    (void)arg;
    printf("producer: filling buffer...\n");
    buffer_produce(req(1));
    buffer_produce(req(2)); /* capacity 2 → buffer full */
    printf("producer: about to block on 3rd produce...\n");
    buffer_produce(req(3)); /* must block until consumer runs */
    printf("producer: 3rd produce done\n");
    return NULL;
}

static void *consumer_after_sleep(void *arg) {
    (void)arg;
    sleep(1); /* give producer time to block */
    request_t r;
    printf("consumer: consuming one (unblocks producer)\n");
    buffer_consume(&r);
    assert(r.fd == 1);
    return NULL;
}

/* Test D: produce blocks when full */
static void test_produce_blocks_when_full(void) {
    buffer_init(2, SCHED_FIFO);
    pthread_t p, c;
    pthread_create(&p, NULL, producer_full, NULL);
    pthread_create(&c, NULL, consumer_after_sleep, NULL);
    pthread_join(p, NULL);
    pthread_join(c, NULL);
    /* drain rest */
    request_t r;
    buffer_consume(&r);
    buffer_consume(&r);
    buffer_destroy();
    printf("Test D OK (produce waits when full)\n");
}

static void *consumer_empty(void *arg) {
    (void)arg;
    request_t r;
    printf("consumer: about to block on empty...\n");
    buffer_consume(&r);
    printf("consumer: got fd=%d\n", r.fd);
    assert(r.fd == 42);
    return NULL;
}

static void *producer_after_sleep(void *arg) {
    (void)arg;
    sleep(1);
    printf("producer: producing 42\n");
    buffer_produce(req(42));
    return NULL;
}

/* Test E: consume blocks when empty */
static void test_consume_blocks_when_empty(void) {
    buffer_init(2, SCHED_FIFO);
    pthread_t c, p;
    pthread_create(&c, NULL, consumer_empty, NULL);
    pthread_create(&p, NULL, producer_after_sleep, NULL);
    pthread_join(c, NULL);
    pthread_join(p, NULL);
    buffer_destroy();
    printf("Test E OK (consume waits when empty)\n");
}

int main(void) {
    request_t r;

    buffer_init(3, SCHED_FIFO);
    buffer_produce(req(10));
    buffer_produce(req(11));
    buffer_produce(req(12));
    assert(buffer_count() == 3);
    buffer_consume(&r); assert(r.fd == 10);
    buffer_consume(&r); assert(r.fd == 11);
    buffer_consume(&r); assert(r.fd == 12);
    printf("Test A OK\n");
    buffer_destroy();

    /* ... keep your wrap + interleave tests (B, C) without -1 checks ... */

    test_produce_blocks_when_full();
    test_consume_blocks_when_empty();

    printf("All Phase 2 buffer tests passed.\n");
    return 0;
}