#include "client_manger.h"
///////////////////// [NOTES] /////////////////////
// [PLANED] hash map for fast look up for users
// [PLANED] iterate via buckets and shift buckets on delete less operations



void init_clients(struct clients *clients){
  clients->clientData = calloc(1, sizeof(int));
  clients->max_clients = 1;
};


void destroy_clients(struct clients *clients){
  for(int i = 0; i < clients->max_clients; i++)
    if(clients->clientData[i].socket_id != -1)
#ifdef WIN32
      closesocket(clients->client_sock[i]);
#else
      close(clients->clientData[i].socket_id);
#endif

  free(clients->clientData);
};


void resize_clients(struct clients *clients) {
  unsigned int new_max_clients =(clients->max_clients * 2 + 1); 
  struct clientData* temp = calloc(new_max_clients, sizeof(struct clientData));
  
  // copy to new buffer
  memcpy(temp, clients->clientData, clients->last_client * sizeof(struct clientData));
  
  // remove old vector
  free(clients->clientData);

  // update clients
  clients->clientData = temp;
  clients->max_clients = new_max_clients;
};


void add_client(struct clients *clients, int socket){
  // check for resize
  if(clients->last_client >= clients->max_clients)
    resize_clients(clients);

  // add client
  clients->clientData[clients->last_client].socket_id = socket;
  clients->last_client++;
}


void delete_client(struct clients *clients, int index){
  // close socket
#ifdef WIN32
  closesocket(clients->client_sock[index]);
#else
  close(clients->clientData[index].socket_id);
#endif

  // clean client data
  memset(&(clients->clientData[index]), 0, sizeof(clients->clientData[index]));

  // shift users
  for(int i = index; i < clients->max_clients - 1; i++)
    if(clients->clientData[i + 1].socket_id != 0)
      clients->clientData[i] = clients->clientData[i + 1];
    
  // update clients
  clients->last_client--;
};