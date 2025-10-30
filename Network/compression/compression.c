#include "compression.h"
#include <string.h>



void compression_rle_compress(char *buffer, char *outbuffer) {
  int out_idx = 0;

  for (int i = 0; buffer[i] != '\0'; ) {
    char c = buffer[i];
    int count = 1;

    while (buffer[i + count] == c && count < 255)
      count++;

    outbuffer[out_idx++] = (char)count;
    outbuffer[out_idx++] = c;          
    i += count;
  }

  outbuffer[out_idx] = '\0';
}




void compression_rle_decompress(char *buffer, char *outbuffer) {
  int out_idx = 0;

  for (int i = 0; buffer[i] != '\0'; i += 2) {
    int count = (unsigned char)buffer[i];
    char c = buffer[i + 1];

    for (int j = 0; j < count; j++) {
      outbuffer[out_idx++] = c;
    }
  }

  outbuffer[out_idx] = '\0';
}













#include <stdint.h>

#define WINDOW_SIZE 64
#define MAX_LENGTH 255

// Compress and store out_len at beginning of outbuffer
void compression_lz4_compress(char *buffer, int in_len, char *outbuffer) {
    int in_idx = 0;
    int out_idx = 4;

    while (in_idx < in_len) {
        int match_offset = 0;
        int match_length = 0;

        int start = (in_idx > WINDOW_SIZE) ? in_idx - WINDOW_SIZE : 0;

        for (int j = start; j < in_idx; j++) {
            int k = 0;
            while (k < MAX_LENGTH && j + k < in_len && in_idx + k < in_len &&
                   buffer[j + k] == buffer[in_idx + k]) {
                k++;
            }
            if (k > match_length) {
                match_length = k;
                match_offset = in_idx - j;
            }
        }

        if (match_length >= 3) {
            outbuffer[out_idx++] = (char)match_offset;
            outbuffer[out_idx++] = (char)match_length;
            outbuffer[out_idx++] = buffer[in_idx + match_length];
            in_idx += match_length + 1;
        } else {
            outbuffer[out_idx++] = 0;
            outbuffer[out_idx++] = 1;
            outbuffer[out_idx++] = buffer[in_idx++];
        }
    }

    // store compressed size at beginning (4 bytes)
    uint32_t compressed_size = out_idx - 4;
    outbuffer[0] = compressed_size & 0xFF;
    outbuffer[1] = (compressed_size >> 8) & 0xFF;
    outbuffer[2] = (compressed_size >> 16) & 0xFF;
    outbuffer[3] = (compressed_size >> 24) & 0xFF;
}


// Decompress using out_len stored in first 4 bytes
void compression_lz4_decompress(char *buffer, char *outbuffer) {
    int in_idx = 4;  // skip first 4 bytes
    int out_idx = 0;

    uint32_t compressed_size = ((uint8_t)buffer[0]) |
                               ((uint8_t)buffer[1] << 8) |
                               ((uint8_t)buffer[2] << 16) |
                               ((uint8_t)buffer[3] << 24);

    while (in_idx < 4 + compressed_size) {
        uint8_t offset = buffer[in_idx++];
        uint8_t length = buffer[in_idx++];
        char next = buffer[in_idx++];

        if (offset == 0) {
            outbuffer[out_idx++] = next;
        } else {
            int copy_start = out_idx - offset;
            for (int i = 0; i < length; i++) {
                outbuffer[out_idx++] = outbuffer[copy_start + i];
            }
            outbuffer[out_idx++] = next;
        }
    }
}
