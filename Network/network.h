#ifndef NETWORK_H
#define NETWORK_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

#define MAX_CONNECT_RETRYS  2000
#define CONNECT_RETRY_DELAY 0.2
#define MAX_CLIENTS 50


struct network_server{
  int server_sock;
  int client_sock[MAX_CLIENTS];
  struct sockaddr_in serv_addr;
};

struct network_client{
  int sock;
  struct sockaddr_in serv_addr;
};

void network_destroy(struct network_client* network_socket);
int network_connect(struct network_client* network_socket);
int network_init(struct network_client* network_socket, const char* ip_addr, int port);
int network_read(struct network_client* network_socket, char* buffer, const unsigned int buffer_size);
void network_send(struct network_client* network_socket, char* buffer, const unsigned int max_message_size);

void network_destroy_server(struct network_server* network_socket);
int network_find_free_socket(struct network_server* network_socket);
int network_listen(struct network_server* network_socket);
int network_init_server(struct network_server* network_socket, const unsigned int port);
int network_read_server(struct network_server* network_socket, unsigned int i, char* buffer, const unsigned int buffer_size);
void network_send_server(struct network_server* network_socket, unsigned int i, char* buffer, const unsigned int max_message_size);

#endif