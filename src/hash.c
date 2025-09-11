#include "../include/hash.h"

uint8_t *get_message_block(FILE *file_ptr, uint64_t *block_size) {
  if (!file_ptr || !block_size) {
    return NULL;
  }

  fseek(file_ptr, 0L, SEEK_END);
  uint64_t data_size = ftell(file_ptr);
  rewind(file_ptr);

  printf("Data size: %llu bytes\n", data_size);

  uint64_t message_block_size =
      ((data_size + 1 + BLOCK_APPENDIX_LENGTH + CHUNK_SIZE - 1) / CHUNK_SIZE) *
      CHUNK_SIZE;

  printf("Message block size: %llu bytes\n\n", message_block_size);

  uint8_t *message_block = (uint8_t *)malloc(message_block_size);
  if (!message_block) {
    perror("malloc failed");
    return NULL;
  }
  memset(message_block, 0, message_block_size);

  for (uint64_t i = 0; i < data_size; i++) {
    if (fread(message_block + i, 1, 1, file_ptr) != 1) {
      perror("Read error");
      return NULL;
    }
  }

  // Append the 1 bit (0x80 at the first unused byte)
  message_block[data_size] = 0x80;

  // Append length (in bits) as 64-bit big-endian
  uint64_t bit_length = data_size * 8;
  for (int i = 0; i < 8; i++) {
    message_block[message_block_size - 8 + i] =
        (bit_length >> (8 * (7 - i))) & 0xFF;
  }

  *block_size = message_block_size;
  return message_block;
}

void print_message_block(uint8_t *message_block, uint64_t message_block_size) {

  printf("Message block: \n");
  for (uint64_t i = 0; i < message_block_size; i++) {
    printf(BYTE_TO_BINARY_PATTERN " ", BYTE_TO_BINARY(message_block[i]));

    if ((i + 1) % 4 == 0) {
      printf("\n");
    }
  }
}
