#include "server.h"

#define PORT 9090
#define BUFFER_SIZE 100
#define NAMESIZE 25

struct network_provider network = {0};
struct clients clients = {0};

char buffer[BUFFER_SIZE + NAMESIZE] = {0};
int running = 1;



////// [REFACTOR] client vector is not optimal creates inf clients and waste memory
void resize_clients(struct clients *clients) {
  unsigned int new_max_clients =(clients->max_clients * 2 + 1); 
  int* temp = calloc(new_max_clients, sizeof(int));
  for(int i = 0; i < clients->max_clients; i++)
    temp[i] = clients->client_sock[i];
  
  free(clients->client_sock);
  clients->client_sock = temp;
  clients->max_clients = new_max_clients;
  printf("clients new max: %d\n", clients->max_clients);
};

void add_client(struct clients *clients, int socket){
  if(clients->last_client >= clients->max_clients)
    resize_clients(clients);
  clients->client_sock[clients->last_client] = socket;
  clients->last_client++;
}

void delete_client(struct clients *clients, int index){
  clients->client_sock[index] = 0;
  for(int i = index; i < clients->max_clients - 1; i++)
    if(clients->client_sock[i + 1] != 0)
      clients->client_sock[i] = clients->client_sock[i + 1];
    else
      break; 
}



void sigint_handler(int sig) {
  network_server_destroy(&network);
  running = 0;
  free(clients.client_sock);
  exit(EXIT_FAILURE);
};





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
    else if(socket == NOCLIENT){}
    else {
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
    
#ifdef _WIN32
    Sleep(50);
#else
    sleep(0.05);
#endif
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
