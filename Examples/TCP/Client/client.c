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
  switch (network_client_init(&network, TCP, ADDR, PORT)){
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

  // Connect to server
  switch (network_client_connect(&network)){
    case CONNECTERROR:
      printf("Can't connect to network!\n");
      network_client_destroy(&network);
      printf("Network destroyed!\n");
      exit(EXIT_FAILURE);
      break;
    default:
      printf("Connected to server!\n");  
  };


  // set noblocking
#ifndef WIN32
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
#endif


  char request[BUFFER_SIZE] = {0};
  printf("> ");
  while (running) {
    char respond[BUFFER_SIZE] = {0};

    // send requests
    if(client_noblocking_get_input(request)){
      network_client_send(&network, request, BUFFER_SIZE);
      printf("> ");
      memset(request, 0, BUFFER_SIZE);
    };

    // get responds
    switch (network_client_read(&network, respond, BUFFER_SIZE)){
      case NODATA:
        break;
      case DISCONNECT:
        printf("\nServer closed the connection.\n");
        running = 0;
        break;
      case ERRORCODE:
        printf("Error on read!\n");
        break;
      default: 
        printf("%s\n> ", respond);
        break;
    };

#ifdef WIN32
    Sleep(0.05);
#else
    sleep(0.05);
#endif
  };

  // clean up
  network_client_destroy(&network);
  printf("Network destroyed!\n");

  return 0;
};

