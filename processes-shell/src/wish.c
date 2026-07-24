#include "args.h"
#include "error.h"
#include "execute.h"
#include "path.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  FILE *input = stdin;
  int interactive = 1;

  if (argc > 2) {
    print_error();
    exit(1);
  }

  if (argc == 2) {
    input = fopen(argv[1], "r");
    if (input == NULL) {
      print_error();
      exit(1);
    }
    interactive = 0;
  }

  init_search_path();

  while (1) {
    if (interactive) {
      printf("wish> ");
      fflush(stdout);
    }

    char *line = get_input(input);
    if (line == NULL) {
      exit(0);
    }

    char **args = parse_args(line);
    free(line);

    if (args == NULL) {
      continue;
    }

    execute_command(args);
    free_args(args);
  }
}
