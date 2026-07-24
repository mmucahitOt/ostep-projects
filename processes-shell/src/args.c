#include "args.h"
#include "error.h"

#include <stdlib.h>
#include <string.h>

void free_args(char **args) {
  if (args == NULL) {
    return;
  }
  for (int i = 0; args[i] != NULL; i++) {
    free(args[i]);
  }
  free(args);
}

char *get_input(FILE *in) {
  char *line = NULL;
  size_t len = 0;
  ssize_t nread;

  nread = getline(&line, &len, in);
  if (nread == -1) {
    free(line);
    return NULL;
  }

  if (nread > 0 && line[nread - 1] == '\n') {
    line[nread - 1] = '\0';
  }

  return line;
}

static int push_token(char ***args, size_t *n, size_t *cap, const char *start,
                      size_t len) {
  if (*n + 1 >= *cap) {
    *cap *= 2;
    char **tmp = realloc(*args, (*cap) * sizeof(char *));
    if (tmp == NULL) {
      return -1;
    }
    *args = tmp;
  }

  char *tok = malloc(len + 1);
  if (tok == NULL) {
    return -1;
  }
  memcpy(tok, start, len);
  tok[len] = '\0';
  (*args)[(*n)++] = tok;
  return 0;
}

/*
 * Split on whitespace; '>' is always its own token (no space required).
 */
char **parse_args(char *line) {
  char *p = line;
  while (*p == ' ' || *p == '\t') {
    p++;
  }
  if (*p == '\0') {
    return NULL;
  }

  size_t cap = 8;
  size_t n = 0;
  char **args = malloc(cap * sizeof(char *));
  if (args == NULL) {
    print_error();
    return NULL;
  }

  while (*p != '\0') {
    while (*p == ' ' || *p == '\t') {
      p++;
    }
    if (*p == '\0') {
      break;
    }

    if (*p == '>' || *p == '&') {
      if (push_token(&args, &n, &cap, p, 1) != 0) {
        free_args(args);
        print_error();
        return NULL;
      }
      p++;
      continue;
    }

    char *start = p;
    while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '>' &&
           *p != '&') {
      p++;
    }
    if (push_token(&args, &n, &cap, start, (size_t)(p - start)) != 0) {
      free_args(args);
      print_error();
      return NULL;
    }
  }

  args[n] = NULL;
  return args;
}

int extract_redirect(char **args, char **redirect_file) {
  *redirect_file = NULL;

  if (args == NULL) {
    return 0;
  }

  int redir_idx = -1;
  for (int i = 0; args[i] != NULL; i++) {
    if (strcmp(args[i], ">") == 0) {
      if (redir_idx != -1) {
        print_error();
        return -1;
      }
      redir_idx = i;
    }
  }

  if (redir_idx == -1) {
    return 0;
  }

  /* no command before '>' */
  if (redir_idx == 0) {
    print_error();
    return -1;
  }

  /* need exactly one filename after '>' */
  if (args[redir_idx + 1] == NULL || args[redir_idx + 2] != NULL) {
    print_error();
    return -1;
  }

  *redirect_file = strdup(args[redir_idx + 1]);
  if (*redirect_file == NULL) {
    print_error();
    return -1;
  }

  free(args[redir_idx]);
  free(args[redir_idx + 1]);
  args[redir_idx] = NULL;
  args[redir_idx + 1] = NULL;
  return 0;
}
