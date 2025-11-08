#include "server.h"



struct network_provider network = {0};
struct clients clients = {0};
int running = 1;


void sigint_handler(int sig) {
  destroy_clients(&clients);
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
  switch (network_server_init(&network, TCP, PORT)){
    case ERRORCODE:
      printf("Can't Initialize Server\n");
      network_server_destroy(&network);
      perror("error");
      exit(EXIT_FAILURE);
      break;
    default: 
      printf("Server Initialized\n");
      printf("Server is running\n");
      break;
  };

  // Init clients
  init_clients(&clients);

  while (running) {
    // listen for new connections
    listen_for_connections(&network, &clients);

    // responds
    for(int i = 0; i < clients.last_client; i++){
      char buffer[BUFFER_SIZE + NAMESIZE] = {0};

      if(clients.clientData[i].socket_id > 0){

        // read requests
        int valread = network_server_read(&clients.clientData[i].socket_id, buffer, BUFFER_SIZE + NAMESIZE);
      
        // respond to request
        switch(valread){
          case DISCONNECT:
            disconnect_user(&clients, i);
            break;
          case NODATA:
            break;
          default:
            respond(buffer, &clients, i);
          break;
        };

      };
    };

#ifdef WIN32
    Sleep(0.05);
#else
    sleep(0.05);
#endif
  };

  // clean up
  destroy_clients(&clients);
  network_server_destroy(&network);
  printf("Clients destroyed!\n");
  printf("Server destroyed!\n");

  return 0;
};
