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

uint8_t *get_message_block(FILE *file_ptr, uint64_t *message_size) {
  if (!file_ptr || !message_size) {
    return NULL;
  }

  fseek(file_ptr, 0L, SEEK_END);
  const uint64_t data_size = ftell(file_ptr);
  rewind(file_ptr);

  printf("Data size: %" PRIu64 " bytes\n", data_size);

  const uint64_t message_block_size =
      ((data_size + 1 + BLOCK_APPENDIX_LENGTH + CHUNK_SIZE - 1) / CHUNK_SIZE) *
      CHUNK_SIZE;

  printf("Message block size: %" PRIu64 " bytes\n\n", message_block_size);

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
  uint32_t a = H0[0];
  uint32_t b = H0[1];
  uint32_t c = H0[2];
  uint32_t d = H0[3];
  uint32_t e = H0[4];
  uint32_t f = H0[5];
  uint32_t g = H0[6];
  uint32_t h = H0[7];

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
}

void sha256_update(SHA256_Ctx *ctx, const uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    ctx->data[ctx->data_length] = data[i];
    ctx->data_length++;
    if (ctx->data_length == 64) {
      uint32_t message_schedule[64];
      decompose_block(0, message_schedule, ctx->data, 64);
      compute_hash(ctx, message_schedule);
      ctx->bit_length += 512; // processed 512 bits
      ctx->data_length = 0;
    }
  }
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
