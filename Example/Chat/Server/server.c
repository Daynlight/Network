#include "server.h"

struct network_provider network = {0};
struct clients clients = {0};
struct clientData clientData = {0};
char buffer[BUFFER_SIZE + NAMESIZE] = {0};
int running = 1;


#include "requests.h"


void sigint_handler(int sig) {
  destroy_clients(&clients);
  network_server_destroy(&network);
  printf("Clients destroyed!\n");
  printf("Server destroyed!\n");
  running = 0;
  exit(EXIT_FAILURE);
};


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
    // listen for new connections
    listen_for_connections();

    // responds
    for(int i = 0; i < clients.last_client; i++){
      memset(buffer, 0, BUFFER_SIZE + NAMESIZE);
      if(clients.clientData[i].socket_id > 0){

        // read requests
        int valread = network_server_read(&clients.clientData[i].socket_id, buffer, BUFFER_SIZE + NAMESIZE);
        char decompressed_data[BUFFER_SIZE + NAMESIZE];
        compression_rle_decompress(buffer, decompressed_data);
      
        // respond to request
        switch(valread){
        case DISCONNECT:
          disconnect_user(i);
          break;
        case NODATA:
          break;
        default:
          respond(decompressed_data, i);
        break;
        };

      };
    };

    sleep(0.5);
  };

  // clean up
  destroy_clients(&clients);
  network_server_destroy(&network);
  printf("Clients destroyed!\n");
  printf("Server destroyed!\n");

  return 0;
};
