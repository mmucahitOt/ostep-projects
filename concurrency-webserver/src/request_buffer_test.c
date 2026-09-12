#include "request_buffer.h"
#include <assert.h>
#include <stdio.h>

static request_t req(int fd) {
  request_t r;
  r.fd = fd;
  r.filesize = -1;
  return r;
}

int main(void) {
  request_t r;

  /* Test A: FIFO order */
  buffer_init(3, SCHED_FIFO);
  assert(buffer_produce(req(10)) == 0);
  assert(buffer_produce(req(11)) == 0);
  assert(buffer_produce(req(12)) == 0);
  assert(buffer_produce(req(99)) == -1); /* full */
  assert(buffer_count() == 3);

  assert(buffer_consume(&r) == 0 && r.fd == 10);
  assert(buffer_consume(&r) == 0 && r.fd == 11);
  assert(buffer_consume(&r) == 0 && r.fd == 12);
  assert(buffer_consume(&r) == -1); /* empty */
  printf("Test A OK (FIFO + full/empty)\n");
  buffer_destroy();

  /* Test B: wrap around */
  buffer_init(2, SCHED_FIFO);
  assert(buffer_produce(req(1)) == 0);
  assert(buffer_produce(req(2)) == 0);
  assert(buffer_consume(&r) == 0 && r.fd == 1);
  assert(buffer_produce(req(3)) == 0); /* uses freed slot */
  assert(buffer_consume(&r) == 0 && r.fd == 2);
  assert(buffer_consume(&r) == 0 && r.fd == 3);
  printf("Test B OK (ring wrap)\n");
  buffer_destroy();

  /* Test C: interleave */
  buffer_init(4, SCHED_FIFO);
  assert(buffer_produce(req(7)) == 0);
  assert(buffer_produce(req(8)) == 0);
  assert(buffer_consume(&r) == 0 && r.fd == 7);
  assert(buffer_produce(req(9)) == 0);
  assert(buffer_consume(&r) == 0 && r.fd == 8);
  assert(buffer_consume(&r) == 0 && r.fd == 9);
  printf("Test C OK (interleave)\n");
  buffer_destroy();

  printf("All Phase 1 buffer tests passed.\n");
  return 0;
}