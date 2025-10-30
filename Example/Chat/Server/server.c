#include "server.h"

#define PORT 9090
#define BUFFER_SIZE 100
#define NAMESIZE 25

struct network_provider network = {0};
struct clients clients = {0};

char buffer[BUFFER_SIZE + NAMESIZE] = {0};
int running = 1;





void sigint_handler(int sig) {
  network_server_destroy(&network);
  running = 0;
  free(clients.client_sock);
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

  printf("Server is running\n");

  if(network_server_init(&network, PORT) == ERRORCODE)
    exit(EXIT_FAILURE);

  clients.client_sock = calloc(1, sizeof(int));
  clients.max_clients = 1;

  while (running) {
    // connect 
    int socket = network_server_listen(&network);
    if(socket == ERRORCODE)
      printf("Cant connect client!\n");
    else if(socket >= SUCCESS){
      char ip[INET_ADDRSTRLEN];
      network_get_client_ip(&socket, ip);
      add_client(&clients, socket);
      printf("Client connected ip: %s\n", ip);
    };

    // broadcast
    for(int i = 0; i < clients.max_clients; i++){
      memset(buffer, 0, BUFFER_SIZE + NAMESIZE);
      if(clients.client_sock[i] > 0){
        int valread = network_server_read(&clients.client_sock[i], buffer,  BUFFER_SIZE + NAMESIZE);
        if(valread > 0){
          printf("%s\n", buffer);
          network_server_broadcast(&network, clients.client_sock, clients.max_clients, i, buffer, BUFFER_SIZE + NAMESIZE);
        }
        else if(valread == DISCONNECT){
          close(clients.client_sock[i]);
          delete_client(&clients, i);
          printf("Client disconnected\n");
        };
      };
    };
    
    client_sleep(0.05);
  };

  network_server_destroy(&network);

  for(int i = 0; i < clients.max_clients; i++)
    if(clients.client_sock[i] != -1)
#ifdef WIN32
      closesocket(clients.client_sock[i]);
#else
      close(clients.client_sock[i]);
#endif

  free(clients.client_sock);

  return 0;
}
