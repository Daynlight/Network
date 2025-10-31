#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define WINDOW_SIZE 255  // look-back window
#define MAX_LENGTH 255   // max match length

void compression_rle_compress(char* buffer, char *outbuffer);
void compression_rle_decompress(char* buffer, char *outbuffer);

void compression_lz4_compress(char* buffer, int in_len, char* outbuffer);
void compression_lz4_decompress(char* buffer, char* outbuffer);

#endif