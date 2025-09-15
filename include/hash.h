#ifndef HASH_H
#define HASH_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 64
#define BLOCK_APPENDIX_LENGTH 8
#define MESSAGE_SCHEDULE_LENGTH 64

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                   \
  ((byte) & 0x80 ? '1' : '0'), ((byte) & 0x40 ? '1' : '0'),                    \
      ((byte) & 0x20 ? '1' : '0'), ((byte) & 0x10 ? '1' : '0'),                \
      ((byte) & 0x08 ? '1' : '0'), ((byte) & 0x04 ? '1' : '0'),                \
      ((byte) & 0x02 ? '1' : '0'), ((byte) & 0x01 ? '1' : '0')

uint8_t *get_message_block(FILE *file_ptr, uint64_t *block_size);
void print_message_block(uint8_t *message_block, uint64_t block_size);
void decompose_block(uint64_t chunk_index, uint32_t *message_schedule,
                     uint8_t *message_block, uint64_t block_size);
#endif // !HASH_H
