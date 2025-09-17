#include "../include/hash.h"
#include <inttypes.h>
#include <stdint.h>

#define BLOCK_APPENDIX_LENGTH 8

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                   \
  ((byte) & 0x80 ? '1' : '0'), ((byte) & 0x40 ? '1' : '0'),                    \
      ((byte) & 0x20 ? '1' : '0'), ((byte) & 0x10 ? '1' : '0'),                \
      ((byte) & 0x08 ? '1' : '0'), ((byte) & 0x04 ? '1' : '0'),                \
      ((byte) & 0x02 ? '1' : '0'), ((byte) & 0x01 ? '1' : '0')

static const uint32_t H0[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                               0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

void sha256_init(SHA256_Ctx *ctx) {
  for (uint32_t i = 0; i < 8; i++) {
    ctx->state[i] = H0[i];
  }
  ctx->bit_length = 0;
  ctx->data_length = 0;
  memset(ctx->data, 0, sizeof(ctx->data));
}

void print_message_block(const uint8_t *message_block,
                         const uint64_t message_block_size) {
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

static inline uint32_t sigma0(uint32_t x) {
  return right_rotate(x, 7) ^ right_rotate(x, 18) ^ (x >> 3);
}

static inline uint32_t sigma1(uint32_t x) {
  return right_rotate(x, 17) ^ right_rotate(x, 19) ^ (x >> 10);
}

static inline uint32_t Sigma0(uint32_t x) {
  return right_rotate(x, 2) ^ right_rotate(x, 13) ^ (x >> 22);
}

static inline uint32_t Sigma1(uint32_t x) {
  return right_rotate(x, 6) ^ right_rotate(x, 11) ^ (x >> 25);
}

static inline uint32_t choice(uint32_t w1, uint32_t w2, uint32_t w3) {
  return (w1 & w2) ^ ((~w1) & w3);
}

static inline uint32_t majority(uint32_t w1, uint32_t w2, uint32_t w3) {
  return (w1 & w2) ^ (w1 & w3) ^ (w2 & w3);
}

void compute_hash(SHA256_Ctx *ctx, uint32_t *message_schedule) {
  uint32_t a = ctx->state[0];
  uint32_t b = ctx->state[1];
  uint32_t c = ctx->state[2];
  uint32_t d = ctx->state[3];
  uint32_t e = ctx->state[4];
  uint32_t f = ctx->state[5];
  uint32_t g = ctx->state[6];
  uint32_t h = ctx->state[7];

  for (uint32_t i = 0; i < MESSAGE_SCHEDULE_LENGTH; i++) {
    uint32_t Temp1 =
        h + Sigma1(e) + choice(e, f, g) + K[i] + message_schedule[i];
    uint32_t Temp2 = Sigma0(a) + majority(a, b, c);
    h = g;
    g = f;
    f = e;
    e = d + Temp1;
    d = c;
    c = b;
    b = a;
    a = Temp1 + Temp2;
  }
  ctx->state[0] += a;
  ctx->state[1] += b;
  ctx->state[2] += c;
  ctx->state[3] += d;
  ctx->state[4] += e;
  ctx->state[5] += f;
  ctx->state[6] += g;
  ctx->state[7] += h;
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
    message_schedule[j] =
        sigma1(message_schedule[j - 2]) + message_schedule[j - 7] +
        sigma0(message_schedule[j - 15]) + message_schedule[j - 16];
  }
}

void sha256_update(SHA256_Ctx *ctx, const uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    ctx->data[ctx->data_length++] = data[i];
    if (ctx->data_length == 64) {
      uint32_t message_schedule[64];
      decompose_block(0, message_schedule, ctx->data, 64);
#ifdef DEBUG
      print_message_block(data, len);
#endif /* ifdef DEBUG */
      compute_hash(ctx, message_schedule);
      ctx->data_length = 0;
    }
  }
  ctx->bit_length += len * 8;
}

uint64_t sha256_final(SHA256_Ctx *ctx, uint8_t *hash) {
  size_t i = ctx->data_length;

  // Step 1: append the 0x80 bit
  ctx->data[i++] = 0x80;

  // Step 2: if not enough space for length (8 bytes), pad this block and
  // process
  if (i > 56) {
    while (i < 64)
      ctx->data[i++] = 0x00;
    uint32_t message_schedule[64];
    decompose_block(0, message_schedule, ctx->data, 64);
    compute_hash(ctx, message_schedule);
    i = 0;
  }

  // Step 3: pad with zeros until 56 bytes
  while (i < 56)
    ctx->data[i++] = 0x00;

  // Step 4: append the 64-bit big-endian length
  uint64_t bit_len = ctx->bit_length;
  for (int j = 7; j >= 0; j--) {
    ctx->data[i++] = (uint8_t)(bit_len >> (8 * j));
  }

  // Step 5: final block
  uint32_t message_schedule[64];
  decompose_block(0, message_schedule, ctx->data, 64);
  compute_hash(ctx, message_schedule);

  // Step 6: produce the hash
  for (int j = 0; j < 8; j++) {
    hash[j * 4 + 0] = (ctx->state[j] >> 24) & 0xff;
    hash[j * 4 + 1] = (ctx->state[j] >> 16) & 0xff;
    hash[j * 4 + 2] = (ctx->state[j] >> 8) & 0xff;
    hash[j * 4 + 3] = (ctx->state[j]) & 0xff;
  }

  return 32; // SHA-256 always 32 bytes
}
