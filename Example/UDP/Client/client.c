#include "client.h"



struct network_provider network = {0};
int running = 1;


void sigint_handler(int sig){
  network_client_destroy(&network);
  printf("Network destroyed!\n");
  running = 0;
  exit(EXIT_FAILURE);
};


int main(){
  // SIGINT callback
  signal(SIGINT, sigint_handler);
  
  // Init Client
  switch (network_client_init(&network, UDP, ADDR, PORT)){
    case CONNECTERROR:
      printf("Can't init network\n");
      exit(EXIT_FAILURE);
      break;
    case DNSERROR:
      printf("Can't find ip from dns!\n");
      exit(EXIT_FAILURE);
      break;
    default: 
      printf("Client initalized!\n");
      break;
  };

  // set noblocking
#ifndef WIN32
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
#endif


  printf("> ");
  while (running) {
    char request[BUFFER_SIZE] = {0};
    
    // get request
    if(client_noblocking_get_input(request)){
      char respond[BUFFER_SIZE] = {0};
      // send request
      switch(network_client_send_request(&network, request, BUFFER_SIZE, respond, 20, 100)){
        case ERRORCODE:
          printf("Can't get respond\n");
          break;
        default:
          printf("%s", respond);
          break;
      }
      
      printf("\n> ");
    }
    sleep(0.05);
  };

  // clean up
  network_client_destroy(&network);
  printf("Network destroyed!\n");

  return 0;
};

