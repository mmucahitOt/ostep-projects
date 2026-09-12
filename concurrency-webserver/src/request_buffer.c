#include "request_buffer.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


static request_t      *slots = NULL;
static int             capacity = 0;
static int             count = 0;
static int             in_idx = 0;   /* next produce index */
static int             out_idx = 0;  /* next consume index */
static sched_policy_t policy = SCHED_FIFO;

void buffer_init(int cap, sched_policy_t pol) {
  assert(cap >= 1);
  buffer_destroy(); /* safe if called twice */
  slots = malloc(sizeof(request_t) * (size_t)cap);
  assert(slots != NULL);
  capacity = cap;
  count = 0;
  in_idx = 0;
  out_idx = 0;
  policy = pol;
}

void buffer_destroy(void) {
  free(slots);
  slots = NULL;
  capacity = count = in_idx = out_idx = 0;
}

int buffer_produce(request_t r) {
  assert(slots != NULL);
  if (count == capacity)
      return -1;
  slots[in_idx] = r;
  in_idx = (in_idx + 1) % capacity;
  count++;
  return 0;
}

int buffer_consume(request_t *out) {
  assert(slots != NULL);
  assert(out != NULL);
  if (count == 0)
      return -1;

  /* Phase 1+4: FIFO. Phase 5: if SCHED_SFF, pick min filesize index instead */
  (void)policy; /* unused until SFF */

  *out = slots[out_idx];
  out_idx = (out_idx + 1) % capacity;
  count--;
  return 0;
}

int buffer_count(void) { return count; }
int buffer_capacity(void) { return capacity; }