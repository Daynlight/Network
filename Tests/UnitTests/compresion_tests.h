#ifndef COMPRESSION_TESTS_H
#define COMPRESSION_TESTS_H

#include <stdio.h>
#include "string.h"

#include "../../Network/compression/compression.h"

int unittests_compression_compres_decompress(int *total, int *passed){
  (*total)++;
  char data[] = "HelloHelloWorldWorld";
  char compressed_data[1024];
  char decompressed_data[1024];

  printf("original data: %s\n", data);
  
  compress(data, compressed_data);
  decompress(compressed_data, decompressed_data);
  
  printf("compressed data: %s\n", compressed_data);
  printf("decompressed data: %s\n", decompressed_data);

  if(!strcmp(data, decompressed_data)) (*passed)++;
}



void unittests_compression_run_all(){
  int total = 0;
  int passed = 0;
  
  unittests_compression_compres_decompress(&total, &passed);

  printf("////////// compression_tests //////////\n");
  printf("%d/%d passed\n", passed, total);
};

#endif