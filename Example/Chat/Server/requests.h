#ifndef REQUESTS_H
#define REQUESTS_H

#include "../../Macro.h"

#include "network.h"
#include "client_manger/client_manger.h"


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
  
  // broadcast disconnect message
  for(int j = 0; j < clients->last_client; j++)
    network_server_send(&(clients->clientData[j].socket_id), message, BUFFER_SIZE + NAMESIZE);
  
  // print on server
  printf("%s\n", message);
};

void public_message(char *decompressed_message, struct clients *clients, int i){
  char message[NAMESIZE + BUFFER_SIZE] = {0};
  
  // create message
  strcat(message, clients->clientData[i].name);
  strcat(message, ": ");
  strcat(message, decompressed_message + 1);

  // broadcast message
  for(int j = 0; j < clients->last_client; j++)
    if(i != j && clients->clientData[j].socket_id > 0)
      network_server_send(&(clients->clientData[j].socket_id), message, BUFFER_SIZE + NAMESIZE);

  // print on server
  printf("public message: %s\n", message);
};

void private_message(char *decompressed_message, struct clients *clients, int i){
  char message[NAMESIZE + BUFFER_SIZE] = {0};
  
  // get user name
  char* first = strchr(decompressed_message, '@'); 
  char* last = strchr(decompressed_message + 2, '@');
  int length = last - first;
  char target_name[NAMESIZE] = {0};
  strncpy(target_name, decompressed_message + 2, length - 1);
  
  // make message
  strcat(message, clients->clientData[i].name);
  strcat(message, " <priv>: ");
  strcat(message, decompressed_message + length + 2);

  // find and send to all target_name
  for(int j = 0; j < clients->last_client; j++)
    if(i != j && clients->clientData[j].socket_id > 0 && strcmp(clients->clientData[j].name, target_name) == 0){
      network_server_send(&(clients->clientData[j].socket_id), message, BUFFER_SIZE + NAMESIZE);
      break;
    };
      
  // print on server
  printf("private message to %s> %s\n", target_name, message);
};

void list_users(struct clients *clients, int i){
  for(int j = 0; j < clients->last_client; j++){
    // create respond
    char respond[NAMESIZE + BUFFER_SIZE] = {0};
    respond[0] = '\n';
    strcat(respond, "* ");
    strcat(respond, clients->clientData[j].name);

    // send respond
    network_server_send(&clients->clientData[i].socket_id, respond, NAMESIZE + BUFFER_SIZE);
  };
  printf("%s listed users\n", clients->clientData[i].name);
};




void respond(char *decompressed_message, struct clients *clients, int i){
  switch(decompressed_message[0]){
    case '1':                                           // Register   -   1<Name>
    register_user(decompressed_message, clients, i);
    break;
  case '2':                                             // Public Message   -   2<Message>
    public_message(decompressed_message, clients, i);
    break;
  case '3':                                               // Private Message   -   3@<User>@<Message>
    private_message(decompressed_message, clients, i);
    break;
  case '4':
    list_users(clients, i);
    default:
      break;
  };
};

#endif