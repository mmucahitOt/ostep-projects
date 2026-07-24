#include "path.h"
#include "args.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char **search_path = NULL;

void init_search_path(void) {
  search_path = malloc(2 * sizeof(char *));
  if (search_path == NULL) {
    print_error();
    exit(1);
  }
  search_path[0] = strdup("/bin");
  if (search_path[0] == NULL) {
    free(search_path);
    search_path = NULL;
    print_error();
    exit(1);
  }
  search_path[1] = NULL;
}

char *find_command(char *command) {
  if (search_path == NULL) {
    return NULL;
  }

  for (int i = 0; search_path[i] != NULL; i++) {
    char *path =
        malloc(strlen(search_path[i]) + 1 + strlen(command) + 1);
    if (path == NULL) {
      return NULL;
    }

    sprintf(path, "%s/%s", search_path[i], command);

    if (access(path, X_OK) == 0) {
      return path;
    }

    free(path);
  }

  return NULL;
}

void path_command(char **dirs) {
  free_args(search_path);
  search_path = NULL;

  size_t n = 0;
  if (dirs != NULL) {
    while (dirs[n] != NULL) {
      n++;
    }
  }

  search_path = malloc((n + 1) * sizeof(char *));
  if (search_path == NULL) {
    print_error();
    return;
  }

  for (size_t i = 0; i < n; i++) {
    search_path[i] = strdup(dirs[i]);
    if (search_path[i] == NULL) {
      free_args(search_path);
      search_path = NULL;
      print_error();
      return;
    }
  }
  search_path[n] = NULL;
}
