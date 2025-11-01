#ifndef REQUESTS_H
#define REQUESTS_H


#include "network.h"
#include "platform.h"


void client_register(struct network_provider* network, char *name){
  // get name
  printf("name: ");
  client_noblocking_get_input(name);

  // check for incorrect name
  if(strchr(name, '@') != NULL){
    printf("invalid username: %s\n", name);
    exit(EXIT_FAILURE);
  };
  
  // create buffer
  char namebuffer[NAMESIZE] = "1";
  strcat(namebuffer, name);

  // compress name
  char compressed_name[NAMESIZE + BUFFER_SIZE];
  compression_rle_compress(namebuffer, compressed_name);
  network_client_send(network, compressed_name, NAMESIZE + BUFFER_SIZE);    // Register
};


int client_commands(char* request, int *running){
  if (strcmp(request, "exit") == 0) {   // exit command
    running = 0;
    return 1;
  };
  return 0;
};


void send_request(struct network_provider *network, char *request){  
  // check if message is valid
  if(network_check_if_empty_message(request) == NODATA){
    printf("Can't send empty message\n");
    return;
  };

  // send message buffer
  char send_request[NAMESIZE + BUFFER_SIZE] = {0};
  
  if (request[0] == '@'){         // Private Message    
    send_request[0] = '3';
    strcat(send_request + 1, request);
  }
  else{                               // Public Message
    send_request[0] = '2';
    strcat(send_request + 1, request);
  }
    
  // compress request
  char compressed_request[NAMESIZE + BUFFER_SIZE];
  compression_rle_compress(send_request, compressed_request);

  // send request
  network_client_send(network, compressed_request, strlen(compressed_request));
  
  // new request pointer
  printf("> ");
};

#endif