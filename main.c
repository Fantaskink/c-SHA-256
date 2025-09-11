#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input file path>\n", argv[0]);
    return EXIT_FAILURE;
  }

  FILE *file_ptr = fopen(argv[1], "r");
  if (file_ptr == NULL) {
    perror("File not found");
    return EXIT_FAILURE;
  }

  uint64_t message_length;

  char a = 'a';
  printf("%x\n", a);

  return EXIT_SUCCESS;
}
