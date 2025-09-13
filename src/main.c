#include "../include/hash.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

  uint64_t block_size;
  uint8_t *message_block = get_message_block(file_ptr, &block_size);
  print_message_block(message_block, block_size);

  decompose_blocks(message_block, block_size);

  fclose(file_ptr);
  return EXIT_SUCCESS;
}
