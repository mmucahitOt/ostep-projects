#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

void print_file(char *filename) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    printf("wcat: cannot open file\n");
    exit(EXIT_FAILURE);
  }

  char buffer[MAX_BUFFER_SIZE];

  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    printf("%s", buffer);
  }
  fclose(file);
}

int main(int argc, char *argv[]) {

  if (argc == 1) {
    return EXIT_SUCCESS;
  }

  for (int i = 1; i < argc; i++) {
    char *filename = argv[i];
    print_file(filename);
  }

  return EXIT_SUCCESS;
}
