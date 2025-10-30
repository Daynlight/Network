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
