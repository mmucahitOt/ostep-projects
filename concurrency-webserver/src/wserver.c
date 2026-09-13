#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io_helper.h"
#include "request.h"
#include "request_buffer.h"
#include "thread_pool.h"

char default_root[] = ".";

static sched_policy_t parse_sched(const char *s) {
  if (strcmp(s, "FIFO") == 0)
    return SCHED_FIFO;
  if (strcmp(s, "SFF") == 0)
    return SCHED_SFF;
  fprintf(stderr, "unknown schedule '%s' (use FIFO or SFF)\n", s);
  exit(1);
}

int main(int argc, char *argv[]) {
  int c;
  char *root_dir = default_root;
  int port = 10000;
  int threads = 1;
  int buffers = 1;
  sched_policy_t sched = SCHED_FIFO;

  while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1) {
    switch (c) {
    case 'd':
      root_dir = optarg;
      break;
    case 'p':
      port = atoi(optarg);
      break;
    case 't':
      threads = atoi(optarg);
      break;
    case 'b':
      buffers = atoi(optarg);
      break;
    case 's':
      sched = parse_sched(optarg);
      break;
    default:
      fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b "
                      "buffers] [-s FIFO|SFF]\n");
      exit(1);
    }
  }

  if (threads < 1 || buffers < 1) {
    fprintf(stderr, "threads and buffers must be >= 1\n");
    exit(1);
  }

  chdir_or_die(root_dir);

  buffer_init(buffers, sched);
  thread_pool_init(threads);

  int listen_fd = open_listen_fd_or_die(port);
  printf("wserver: port=%d threads=%d buffers=%d sched=%s\n", port, threads,
         buffers, sched == SCHED_SFF ? "SFF" : "FIFO");

  while (1) {
    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    int conn_fd = accept_or_die(listen_fd, (sockaddr_t *)&client_addr,
                                (socklen_t *)&client_len);

    request_t r;
    r.fd = conn_fd;
    r.filesize = -1; /* master does not read the request */
    if (sched == SCHED_SFF)
      r.filesize = request_peek_filesize(conn_fd);

    buffer_produce(r); /* blocks if buffer full */
  }

  return 0;
}