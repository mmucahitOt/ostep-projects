#ifndef ARGS_H
#define ARGS_H

#include <stdio.h>

void free_args(char **args);
char *get_input(FILE *in);
char **parse_args(char *line);

/*
 * Find '>' in args, validate, remove it and the filename from args.
 * On success: returns 0 and sets *redirect_file (NULL if no redirection).
 * On error: prints error, returns -1; *redirect_file is NULL.
 * Caller must free *redirect_file when non-NULL.
 */
int extract_redirect(char **args, char **redirect_file);

#endif
