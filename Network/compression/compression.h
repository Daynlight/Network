#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <stdio.h>

void compression_rle_compress(char* buffer, char *outbuffer);
void compression_rle_decompress(char* buffer, char *outbuffer);

#endif