#include "thread_pool.h"
#include "request_buffer.h"
#include "request.h"
#include "io_helper.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static void *worker_loop(void *arg) {
    (void)arg;
    while (1) {
        request_t r;
        buffer_consume(&r);          /* wait if buffer empty */
        request_handle(r.fd);        /* static or CGI (fork/wait) */
        close_or_die(r.fd);
    }
    return NULL;
}

void thread_pool_init(int nworkers) {
    assert(nworkers >= 1);
    pthread_t *tids = malloc(sizeof(pthread_t) * (size_t)nworkers);
    assert(tids != NULL);

    for (int i = 0; i < nworkers; i++) {
        int rc = pthread_create(&tids[i], NULL, worker_loop, NULL);
        assert(rc == 0);
        pthread_detach(tids[i]);     /* server runs forever; no join */
    }
    /* tids leaked on purpose: process lives until killed */
}