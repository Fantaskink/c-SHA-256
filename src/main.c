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

  fseek(file_ptr, 0L, SEEK_END);
  uint64_t data_size = ftell(file_ptr);

  rewind(file_ptr);

  printf("Data size: %llx\n", data_size);

  return EXIT_SUCCESS;
}
