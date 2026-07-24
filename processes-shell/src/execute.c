#include "execute.h"
#include "args.h"
#include "error.h"
#include "path.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

enum { MAX_PARALLEL = 64 };

void cd_command(char *directory) {
  if (chdir(directory) != 0) {
    print_error();
  }
}

/* Fork and exec; does not wait. Returns pid, or -1 on fork failure. */
static pid_t fork_and_execute(char *full_command_path, char **args,
                              char *redirect_file) {
  pid_t pid = fork();

  if (pid == -1) {
    print_error();
    return -1;
  }

  if (pid == 0) {
    if (redirect_file != NULL) {
      int fd = open(redirect_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd < 0) {
        print_error();
        _exit(1);
      }
      dup2(fd, STDOUT_FILENO);
      dup2(fd, STDERR_FILENO);
      close(fd);
    }

    execv(full_command_path, args);
    print_error();
    _exit(1);
  }

  return pid;
}

/* Run one job (no '&'). External commands are forked; pid stored for later wait. */
static void run_job(char **args, pid_t *pids, char **paths, int *nchildren) {
  if (args == NULL || args[0] == NULL) {
    return;
  }

  char *redirect_file = NULL;
  if (extract_redirect(args, &redirect_file) != 0) {
    return;
  }

  if (args[0] == NULL) {
    free(redirect_file);
    return;
  }

  if (strcmp(args[0], "exit") == 0) {
    if (args[1] != NULL) {
      print_error();
      free(redirect_file);
      return;
    }
    free(redirect_file);
    exit(0);
  }

  if (strcmp(args[0], "cd") == 0) {
    if (args[1] == NULL || args[2] != NULL) {
      print_error();
      free(redirect_file);
      return;
    }
    cd_command(args[1]);
    free(redirect_file);
    return;
  }

  if (strcmp(args[0], "path") == 0) {
    path_command(&args[1]);
    free(redirect_file);
    return;
  }

  char *full_path = find_command(args[0]);
  if (full_path == NULL) {
    print_error();
    free(redirect_file);
    return;
  }

  if (*nchildren >= MAX_PARALLEL) {
    print_error();
    free(full_path);
    free(redirect_file);
    return;
  }

  pid_t pid = fork_and_execute(full_path, args, redirect_file);
  free(redirect_file);

  if (pid == -1) {
    free(full_path);
    return;
  }

  pids[*nchildren] = pid;
  paths[*nchildren] = full_path;
  (*nchildren)++;
}

void execute_command(char **args) {
  if (args == NULL) {
    return;
  }

  pid_t pids[MAX_PARALLEL];
  char *paths[MAX_PARALLEL];
  int nchildren = 0;

  int i = 0;
  while (1) {
    int start = i;
    while (args[i] != NULL && strcmp(args[i], "&") != 0) {
      i++;
    }
    int end = i;

    /* Empty job (e.g. lone '&' or trailing '&') — skip, not an error */
    if (end > start) {
      char *saved = args[end];
      args[end] = NULL;
      run_job(&args[start], pids, paths, &nchildren);
      args[end] = saved;
    }

    if (args[i] != NULL && strcmp(args[i], "&") == 0) {
      i++;
      continue;
    }
    break;
  }

  for (int j = 0; j < nchildren; j++) {
    waitpid(pids[j], NULL, 0);
    free(paths[j]);
  }
}
