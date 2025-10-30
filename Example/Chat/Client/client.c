#include "client.h"

#define PORT 9090
// #define ADDR "tin-s498831.vm.wmi.amu.edu.pl"
#define ADDR "127.0.0.1"

#define BUFFER_SIZE 100
#define NAMESIZE 25

struct network_provider network = {0};
char send_buffer[BUFFER_SIZE] = {0};
char read_buffer[BUFFER_SIZE + NAMESIZE] = {0};
char name[NAMESIZE] = {0};
int running = 1;


#include "platform.h"





void sigint_handler(int sig){
  network_client_destroy(&network);
  printf("Network destroyed!\n");
  running = 0;
  exit(EXIT_FAILURE);
};






int main(){
  signal(SIGINT, sigint_handler);

  switch (network_client_init(&network, ADDR, PORT)){
    case CONNECTERROR:
      printf("Can't init network\n");
      exit(EXIT_FAILURE);
      break;
    case DNSERROR:
      printf("Can't find ip from dns!\n");
      exit(EXIT_FAILURE);
      break;
    default: 
      printf("Server initalized!\n");
      break;
  };

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


  printf("name: ");
  fgets(name, NAMESIZE -1, stdin);
  name[strcspn(name, "\n")] = ':';
  strcat(name, " ");
  printf("logged as %s\n", name);


#ifndef WIN32
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
#endif


  printf("> ");
  while (running) {
    client_get_input();

    switch (network_client_read(&network, read_buffer, BUFFER_SIZE + NAMESIZE)){
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
        char decompressed_data[BUFFER_SIZE + NAMESIZE];
        compression_lz4_decompress(read_buffer, decompressed_data);
        printf("%s\n> ", decompressed_data);
        memset(read_buffer, 0, BUFFER_SIZE + NAMESIZE);
        break;
    };

    client_sleep(0.05);
  };


  network_client_destroy(&network);

  printf("Network destroyed!\n");

  return 0;
};

