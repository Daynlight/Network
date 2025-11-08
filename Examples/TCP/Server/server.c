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


    // operate all clients
    for(int i = 0; i < clients.last_client; i++){
      char message[BUFFER_SIZE + NAMESIZE] = {0};

      if(clients.clientData[i].socket_id > 0){
        // get requests
        int valread = network_server_read(&clients.clientData[i].socket_id, message, BUFFER_SIZE + NAMESIZE);
      
        // send respond
        switch(valread){
          case DISCONNECT:
              // disconnect user
              delete_client(&clients, i);
              printf("User disconnected\n");
            break;
          case NODATA:
            break;
          default:  
            // broadcast as respond
            for(int j = 0; j < clients.last_client; j++)
              if(i != j && clients.clientData[j].socket_id > 0)
                network_server_send(&(clients.clientData[j].socket_id), message, BUFFER_SIZE + NAMESIZE);
            printf("message: %s\n", message);
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
