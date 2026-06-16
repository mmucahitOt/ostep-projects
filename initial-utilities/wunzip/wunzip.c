#include <stdio.h>
#include <stdlib.h>

static void decompress_file(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("%s\n", "wunzip: cannot open file");
    exit(EXIT_FAILURE);
  }

  int count;
  char c;
  while (fread(&count, sizeof(int), 1, file) == 1 &&
         fread(&c, sizeof(char), 1, file) == 1) {
    for (int i = 0; i < count; i++) {
      printf("%c", c);
    }
  }

  fclose(file);
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("%s\n", "wunzip: file1 [file2 ...]");
    exit(EXIT_FAILURE);
  }

  for (int i = 1; i < argc; i++) {
    decompress_file(argv[i]);
  }

  return 0;
}
