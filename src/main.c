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
  /*
    uint64_t num_chunks = message_block_size / CHUNK_SIZE;
    uint32_t message_schedule[64];

    for (uint64_t i = 0; i < num_chunks; i++) {
      for (uint32_t j = 0; j < 16; j++) {
        uint8_t b1 = message_block[i * CHUNK_SIZE + j];
        uint8_t b2 = message_block[i * CHUNK_SIZE + j + 1];
        uint8_t b3 = message_block[i * CHUNK_SIZE + j + 2];
        uint8_t b4 = message_block[i * CHUNK_SIZE + j + 3];
        message_schedule[j] = ((uint32_t)b1 << 24) | ((uint32_t)b2 << 16) |
                              ((uint32_t)b3 << 8) | (uint32_t)b4;
      }
    }
    */
  fclose(file_ptr);
  return EXIT_SUCCESS;
}
