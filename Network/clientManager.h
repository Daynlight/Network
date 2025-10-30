//////////////////////////////////////////////////////////////////
////////////////////////////// IDEA //////////////////////////////
//////////////////////////////////////////////////////////////////
// Client Manager is used to optimal se of clients
// It uses hash_map for fast look up


#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <stdlib.h>
#include "network.h"


struct clients{
  int* client_sock;
  unsigned int max_clients;
  unsigned int last_client;
};


void init_clients(struct clients *clients);
void destroy_clients(struct clients *clients);
void resize_clients(struct clients *clients);
void add_client(struct clients *clients, int socket);
void delete_client(struct clients *clients, int index);


#endif