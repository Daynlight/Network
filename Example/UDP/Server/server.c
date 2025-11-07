#include "server.h"



struct network_provider network = {0};
int running = 1;


void sigint_handler(int sig) {
  network_server_destroy(&network);
  printf("Clients destroyed!\n");
  printf("Server destroyed!\n");
  running = 0;
  exit(EXIT_FAILURE);
};


int main() {
  // SIGINT callback
  signal(SIGINT, sigint_handler);

  // Init server
  switch (network_server_init(&network, UDP, PORT)){
    case ERRORCODE:
      printf("Can't Initialize Server\n");
      network_server_destroy(&network);
      exit(EXIT_FAILURE);
      break;
    default: 
      printf("Server Initialized\n");
      printf("Server is running\n");
      break;
  };

  while (running) {
    struct sockaddr_in client_socket = {0};
    char request[BUFFER_SIZE] = {0};

    // get request
    int val = network_server_read_from(&network, &client_socket, request, BUFFER_SIZE);

    // respond
    if(val == SUCCESS){
      // generate respond
      printf("user request: %s\n", request);

      // send respond

    };

    sleep(0.05);
  };

  // clean up
  network_server_destroy(&network);
  printf("Clients destroyed!\n");
  printf("Server destroyed!\n");

  return 0;
};
