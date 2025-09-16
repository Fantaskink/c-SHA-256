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

  uint64_t message_size;
  uint8_t *message_block = get_message_block(file_ptr, &message_size);
  print_message_block(message_block, message_size);

  uint64_t num_chunks = message_size / CHUNK_SIZE;

  for (uint64_t chunk_index = 0; chunk_index < num_chunks; chunk_index++) {
    uint32_t message_schedule[MESSAGE_SCHEDULE_LENGTH];
    decompose_block(chunk_index, message_schedule, message_block, message_size);
  }

  free(message_block);
  fclose(file_ptr);
  return EXIT_SUCCESS;
}
