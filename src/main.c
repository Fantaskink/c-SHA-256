#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                   \
  ((byte) & 0x80 ? '1' : '0'), ((byte) & 0x40 ? '1' : '0'),                    \
      ((byte) & 0x20 ? '1' : '0'), ((byte) & 0x10 ? '1' : '0'),                \
      ((byte) & 0x08 ? '1' : '0'), ((byte) & 0x04 ? '1' : '0'),                \
      ((byte) & 0x02 ? '1' : '0'), ((byte) & 0x01 ? '1' : '0')

#define BLOCK_INCREMENT 64
#define BLOCK_APPENDIX_LENGTH 8

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

  uint64_t message_block_size =
      ((data_size + 1 + BLOCK_APPENDIX_LENGTH + BLOCK_INCREMENT - 1) /
       BLOCK_INCREMENT) *
      BLOCK_INCREMENT;

  printf("Message block size: %llu bytes\n\n", message_block_size);

  uint8_t *message_block = (uint8_t *)malloc(message_block_size);
  if (!message_block) {
    perror("malloc failed");
    fclose(file_ptr);
    return EXIT_FAILURE;
  }
  memset(message_block, 0, message_block_size);

  for (uint64_t i = 0; i < data_size; i++) {
    if (fread(message_block + i, 1, 1, file_ptr) != 1) {
      perror("Read error");
      fclose(file_ptr);
      return EXIT_FAILURE;
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

  printf("Message block: \n");

  for (uint64_t i = 0; i < message_block_size; i++) {
    printf(BYTE_TO_BINARY_PATTERN " ", BYTE_TO_BINARY(message_block[i]));

    if ((i + 1) % 4 == 0) {
      printf("\n");
    }
  }

  fclose(file_ptr);
  return EXIT_SUCCESS;
}
