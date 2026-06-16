#include <stdio.h>
#include <stdlib.h>

static void write_run(int count, char c) {
  fwrite(&count, sizeof(int), 1, stdout);
  fwrite(&c, sizeof(char), 1, stdout);
}

static void compress_file(char *filename, int *has_run, char *current_char,
                          int *count) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("%s\n", "wzip: cannot open file");
    exit(EXIT_FAILURE);
  }

  int c;
  while ((c = fgetc(file)) != EOF) {
    if (!*has_run) {
      *current_char = (char)c;
      *count = 1;
      *has_run = 1;
    } else if ((char)c == *current_char) {
      (*count)++;
    } else {
      write_run(*count, *current_char);
      *current_char = (char)c;
      *count = 1;
    }
  }

  fclose(file);
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("%s\n", "wzip: file1 [file2 ...]");
    exit(EXIT_FAILURE);
  }

  int has_run = 0;
  char current_char = 0;
  int count = 0;

  for (int i = 1; i < argc; i++) {
    compress_file(argv[i], &has_run, &current_char, &count);
  }

  if (has_run) {
    write_run(count, current_char);
  }

  return 0;
}
