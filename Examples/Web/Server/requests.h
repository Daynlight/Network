#ifndef REQUESTS_H
#define REQUESTS_H

#include "../Macro.h"

#include "network.h"
#include "client_manger/client_manger.h"
#include <stdio.h>


void listen_for_connections(struct network_provider* network, struct clients* clients){
  int socket = network_server_listen(network); 
  switch (socket){
    case ERRORCODE:
    printf("Cant connect client!\n");
    break;
  case NOCLIENT:
    break;
  default:
    char ip[INET_ADDRSTRLEN];
    network_get_client_ip(&socket, ip);
    add_client(clients, socket);
    printf("Client connected ip: %s\n", ip);
    break;
  };
};


void register_user(char *decompressed_message, struct clients* clients, int i){
  char name[NAMESIZE];
  strcpy(name, decompressed_message + 1);

  // check for incorrect name
  if(strchr(name, '@') != NULL){
    printf("invalid username: %s\n", name);
    delete_client(clients, i);
    return;
  };

  if(strchr(name, '/') != NULL){
    printf("invalid username: %s\n", name);
    delete_client(clients, i);
    return;
  };

  // check if user already logged
  for(int j = 0; j < clients->last_client; j++)
    if(strcmp(clients->clientData[j].name, name) == 0){
      printf("username already exists: %s\n", name);
      delete_client(clients, i);
      return;
    };

  // save name
  strcpy(clients->clientData[i].name, name);

  // print on server
  printf("user %s registered\n", clients->clientData[i].name);
};


void disconnect_user(struct clients *clients, int i){
  char message[BUFFER_SIZE + NAMESIZE] = {0};

  // create message
  strcat(message, "Client ");
  strcat(message, clients->clientData[i].name);
  strcat(message, " disconnected");
  
  // delete user
  delete_client(clients, i);

  // print on server
  printf("%s\n", message);
};

void respond(char *message, struct clients *clients, int i){
  char send[BUFFER_SIZE] = {0};
  strcat(send, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
  strcat(send, message);
  network_server_send(&clients->clientData[i].socket_id, send, NAMESIZE + BUFFER_SIZE);
};

#endif