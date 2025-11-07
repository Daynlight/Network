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
    struct sockaddr client_socket = {0};
    char request[BUFFER_SIZE] = {0};

    // get request
    int val = network_server_read_from(&network, &client_socket, request, BUFFER_SIZE);

    // respond
    if(val >= SUCCESS){
      // generate respond
      char respond[BUFFER_SIZE] = "received: ";
      strcat(respond, request);
      
      // send respond
      network_server_send_to(&network, &client_socket, respond, BUFFER_SIZE);
      
      // print request
      printf("user request: %s\n", request);
    };

    sleep(0.05);
  };

  // clean up
  network_server_destroy(&network);
  printf("Clients destroyed!\n");
  printf("Server destroyed!\n");

  return 0;
};
