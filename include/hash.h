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
void sha256_update(SHA256_Ctx *ctx, const uint8_t *data, size_t len);
#endif // !HASH_H
