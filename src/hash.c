#include "../include/hash.h"
#include <stdint.h>

uint8_t *get_message_block(FILE *file_ptr, uint64_t *message_size) {
  if (!file_ptr || !message_size) {
    return NULL;
  }

  fseek(file_ptr, 0L, SEEK_END);
  const uint64_t data_size = ftell(file_ptr);
  rewind(file_ptr);

  printf("Data size: %llu bytes\n", data_size);

  const uint64_t message_block_size =
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
  const uint64_t bit_length = data_size * 8;
  for (int i = 0; i < 8; i++) {
    message_block[message_block_size - 8 + i] =
        (bit_length >> (8 * (7 - i))) & 0xFF;
  }

  *message_size = message_block_size;
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

static inline uint32_t right_rotate(uint32_t word, uint32_t n) {
  return (word >> n) | (word << (32 - n));
}

static inline uint32_t sigma(uint32_t word, uint32_t n1, uint32_t n2,
                             uint32_t n3) {
  return right_rotate(word, n1) ^ right_rotate(word, n2) ^
         right_rotate(word, n3);
}

void decompose_block(uint64_t chunk_index, uint32_t *message_schedule,
                     uint8_t *message_block, uint64_t message_size) {
  for (uint32_t j = 0; j < 16; j++) {
    const size_t idx = chunk_index * CHUNK_SIZE + j * 4;
    const uint8_t b1 = message_block[idx];
    const uint8_t b2 = message_block[idx + 1];
    const uint8_t b3 = message_block[idx + 2];
    const uint8_t b4 = message_block[idx + 3];
    message_schedule[j] = ((uint32_t)b1 << 24) | ((uint32_t)b2 << 16) |
                          ((uint32_t)b3 << 8) | (uint32_t)b4;
  }
  for (uint32_t j = 16; j < MESSAGE_SCHEDULE_LENGTH; j++) {
    const size_t idx = j - 16;
    const uint32_t word_1 = message_schedule[0 + idx];
    const uint32_t word_2 = message_schedule[1 + idx];
    const uint32_t word_3 = message_schedule[9 + idx];
    const uint32_t word_4 = message_schedule[14 + idx];
    message_schedule[j] =
        word_1 + sigma(word_2, 7, 18, 3) + word_3 + sigma(word_4, 17, 19, 10);
  }
}

static inline uint32_t choice(uint32_t w1, uint32_t w2, uint32_t w3) {
  return (w1 & w2) ^ ((~w1) & w3);
}

static inline uint32_t majority(uint32_t w1, uint32_t w2, uint32_t w3) {
  return (w1 & w2) ^ (w1 & w3) ^ (w2 & w3);
}

void compute_hash(uint64_t chunk_index, uint32_t *message_schedule,
                  uint8_t *message_block, uint64_t message_size) {}
