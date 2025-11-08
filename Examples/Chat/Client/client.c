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

  // show help command
  help_command();

  // register
  char name[NAMESIZE] = {0};
  client_register(&network, name);
  printf("logged as %s\n", name);

  // set noblocking
#ifndef WIN32
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
#endif


  printf("> ");
  while (running) {
    char request[BUFFER_SIZE + NAMESIZE] = {0};
    char respond[BUFFER_SIZE + NAMESIZE] = {0};

    // requests
    if(client_noblocking_get_input(request))  // read input
      if(!client_commands(request, &running, name)) // client commands
        send_request(&network, request);      // send to server

    // operate responds
    switch (network_client_read(&network, respond, BUFFER_SIZE + NAMESIZE)){
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

