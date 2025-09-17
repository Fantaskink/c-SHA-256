#ifndef HASH_H
#define HASH_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 64
#define MESSAGE_SCHEDULE_LENGTH 64

typedef struct {
  uint32_t state[8];
  uint64_t bit_length;
  uint32_t data_length;
  uint8_t data[64];
} SHA256_Ctx;

void sha256_init(SHA256_Ctx *ctx);
uint8_t *get_message_block(FILE *file_ptr, uint64_t *block_size);
void print_message_block(uint8_t *message_block, uint64_t block_size);
void decompose_block(uint64_t chunk_index, uint32_t *message_schedule,
                     uint8_t *message_block, uint64_t block_size);
#endif // !HASH_H
