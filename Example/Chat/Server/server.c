#include "server.h"

#define PORT 9090
#define BUFFER_SIZE 100
#define NAMESIZE 25

struct network_provider network = {0};
struct clients clients = {0};
char buffer[BUFFER_SIZE + NAMESIZE] = {0};
int running = 1;





void sigint_handler(int sig) {
  destroy_clients(&clients);
  network_server_destroy(&network);
  printf("Clients destroyed!\n");
  printf("Server destroyed!\n");
  running = 0;
  exit(EXIT_FAILURE);
};





#ifdef WIN32
void client_sleep(float time_seconds){
  Sleep(time_seconds * 1000);
};
#else
void client_sleep(float time_seconds){
  sleep(time_seconds);
};
#endif






int main() {
  signal(SIGINT, sigint_handler);

  switch (network_server_init(&network, PORT)){
    case ERRORCODE:
      printf("Can't Initialize Server");
      network_server_destroy(&network);
      exit(EXIT_FAILURE);
      break;
    default: 
      printf("Server Initialized\n");
      printf("Server is running\n");
      break;
  };

  init_clients(&clients);

  while (running) {
    // connect
    int socket = network_server_listen(&network); 
    switch (socket){
      case ERRORCODE:
      printf("Cant connect client!\n");
      break;
    case NOCLIENT:
      break;
    default:
      char ip[INET_ADDRSTRLEN];
      network_get_client_ip(&socket, ip);
      add_client(&clients, socket);
      printf("Client connected ip: %s\n", ip);
      break;
    };

    // broadcast
    for(int i = 0; i < clients.max_clients; i++){
      memset(buffer, 0, BUFFER_SIZE + NAMESIZE);
      if(clients.client_sock[i] > 0){
        int valread = network_server_read(&clients.client_sock[i], buffer,  BUFFER_SIZE + NAMESIZE);
        char decompressed_data[BUFFER_SIZE + NAMESIZE];
        compression_rle_decompress(buffer, decompressed_data);
        switch(valread){
        case DISCONNECT:
          delete_client(&clients, i);
          printf("Client disconnected\n");
          break;
        case NODATA:
          break;
        default:
          printf("%s\n", decompressed_data);
          network_server_broadcast(&network, clients.client_sock, clients.max_clients, i, buffer, BUFFER_SIZE + NAMESIZE);
          break;
        };
      };
    };
    client_sleep(0.05);
  };

  destroy_clients(&clients);
  network_server_destroy(&network);
  printf("Clients destroyed!\n");
  printf("Server destroyed!\n");

  return 0;
};
