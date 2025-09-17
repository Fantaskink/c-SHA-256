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
    fprintf(stderr, "File not found\n");
    return EXIT_FAILURE;
  }

  SHA256_Ctx ctx;
  sha256_init(&ctx);

  uint8_t buffer[1024];
  size_t bytes_read;
  while ((bytes_read = fread(buffer, 1, sizeof(buffer), file_ptr)) > 0) {
    sha256_update(&ctx, buffer, bytes_read);
  }

  fclose(file_ptr);
  return EXIT_SUCCESS;
}
