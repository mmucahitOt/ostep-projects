#include "error.h"

#include <string.h>
#include <unistd.h>

void print_error(void) {
  char error_message[30] = "An error has occurred\n";
  ssize_t n =
      write(STDERR_FILENO, error_message, strlen(error_message));
  (void)n;
}
