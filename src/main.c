#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                   \
  ((byte) & 0x80 ? '1' : '0'), ((byte) & 0x40 ? '1' : '0'),                    \
      ((byte) & 0x20 ? '1' : '0'), ((byte) & 0x10 ? '1' : '0'),                \
      ((byte) & 0x08 ? '1' : '0'), ((byte) & 0x04 ? '1' : '0'),                \
      ((byte) & 0x02 ? '1' : '0'), ((byte) & 0x01 ? '1' : '0')

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input file path>\n", argv[0]);
    return EXIT_FAILURE;
  }

  FILE *file_ptr = fopen(argv[1], "rb");
  if (file_ptr == NULL) {
    perror("File not found");
    return EXIT_FAILURE;
  }

  fseek(file_ptr, 0L, SEEK_END);
  uint64_t data_size = ftell(file_ptr);
  rewind(file_ptr);

  printf("Data size: %llu bytes\n", data_size);

  for (uint64_t i = 0; i < data_size; i++) {
    unsigned char byte;
    if (fread(&byte, 1, 1, file_ptr) != 1) {
      perror("Read error");
      fclose(file_ptr);
      return EXIT_FAILURE;
    }

    printf(BYTE_TO_BINARY_PATTERN " ", BYTE_TO_BINARY(byte));

    if ((i + 1) % 4 == 0) {
      printf("\n");
    }
  }

  fclose(file_ptr);
  return EXIT_SUCCESS;
}
