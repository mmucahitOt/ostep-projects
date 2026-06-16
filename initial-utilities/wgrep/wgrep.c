#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_file_lines_containing_searchterm(char *filename, char *searchterm) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("%s\n", "wgrep: cannot open file");
    exit(EXIT_FAILURE);
  }

  char *buffer = NULL;
  size_t buffer_size = 0;
  while (getline(&buffer, &buffer_size, file) != -1) {
    if (strstr(buffer, searchterm) != NULL) {
      printf("%s", buffer);
    }
  }

  free(buffer);
  fclose(file);
}

void print_files_lines_containing_searchterm(char *searchterm, char **filenames,
                                             int num_files) {
  for (int i = 0; i < num_files; i++) {
    print_file_lines_containing_searchterm(filenames[i], searchterm);
  }
}

void print_stdin_lines_containing_searchterm(char *searchterm) {
  char *buffer = NULL;
  size_t buffer_size = 0;
  while (getline(&buffer, &buffer_size, stdin) != -1) {
    if (strstr(buffer, searchterm) != NULL) {
      printf("%s", buffer);
    }
  }
  free(buffer);
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("%s\n", "wgrep: searchterm [file ...]");
    exit(EXIT_FAILURE);
  }

  char *searchterm = argv[1];

  if (strlen(searchterm) == 0) {
    return EXIT_SUCCESS;
  }

  if (argc == 2) {
    print_stdin_lines_containing_searchterm(searchterm);
  } else {
    print_files_lines_containing_searchterm(searchterm, argv + 2, argc - 2);
  }
}