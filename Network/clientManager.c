#include "clientManager.h"

void init_clients(struct clients *clients){
  clients->client_sock = calloc(1, sizeof(int));
  clients->max_clients = 1;
}

void destroy_clients(struct clients *clients){
  for(int i = 0; i < clients->max_clients; i++)
    if(clients->client_sock[i] != -1)
#ifdef WIN32
      closesocket(clients->client_sock[i]);
#else
      close(clients->client_sock[i]);
#endif

  free(clients->client_sock);
}

////// [REFACTOR] client vector is not optimal creates inf clients and waste memory
void resize_clients(struct clients *clients) {
  unsigned int new_max_clients =(clients->max_clients * 2 + 1); 
  int* temp = calloc(new_max_clients, sizeof(int));
  for(int i = 0; i < clients->max_clients; i++)
    temp[i] = clients->client_sock[i];
  
  free(clients->client_sock);
  clients->client_sock = temp;
  clients->max_clients = new_max_clients;
};

void add_client(struct clients *clients, int socket){
  if(clients->last_client >= clients->max_clients)
    resize_clients(clients);

  clients->client_sock[clients->last_client] = socket;
  clients->last_client++;
}

void delete_client(struct clients *clients, int index){
#ifdef WIN32
  closesocket(clients->client_sock[index]);
#else
  close(clients->client_sock[index]);
#endif

  // shift right
  clients->client_sock[index] = 0;
  for(int i = index; i < clients->max_clients - 1; i++)
    if(clients->client_sock[i + 1] != 0)
      clients->client_sock[i] = clients->client_sock[i + 1];
    else
      break; 
};