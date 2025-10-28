#ifndef NETWORK_H
#define NETWORK_H




#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#ifdef WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <netdb.h> 
  #include <arpa/inet.h>
#endif



#ifndef MAX_CONNECT_RETRYS
#define MAX_CONNECT_RETRYS  2000
#endif
#ifndef CONNECT_RETRY_DELAY
#define CONNECT_RETRY_DELAY 0.2
#endif
#ifndef INITIAL_MAX_CLIENTS
#define INITIAL_MAX_CLIENTS 1
#endif




enum NetworkCodes{
  NOFREESOCKET = -4,
  NODATA = -3,
  DISCONNECT = -2,
  ERRORCODE = -1,
  NOCLIENT = -3,
  SUCCESS = 0
};




struct network_provider{
  int sock;
  struct sockaddr_in serv_addr;
};




//////////////////////////////////////////////////////////////////
///////////////////////////// Client /////////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_client_init(struct network_provider* network_provider, const char* addr, int port);
enum NetworkCodes network_client_destroy(struct network_provider* network_provider);
enum NetworkCodes network_client_connect(struct network_provider* network_provider);
int network_client_read(struct network_provider* network_provider, char* buffer, const unsigned int buffer_size);
enum NetworkCodes network_client_send(struct network_provider* network_provider, char* buffer, const unsigned int max_message_size);




//////////////////////////////////////////////////////////////////
///////////////////////////// Server /////////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_server_init(struct network_provider* network_provider, const unsigned int port);
enum NetworkCodes network_server_destroy(struct network_provider* network_provider);
int network_server_listen(struct network_provider* network_provider);
int network_server_read(int* socket, char *buffer, const unsigned int buffer_size);
enum NetworkCodes network_server_send(int* socket, char *buffer, const unsigned int max_message_size);
enum NetworkCodes network_server_broadcast(struct network_provider *network_provider, int* clients_sockets, 
                  unsigned int max_clients, const unsigned int i, char *buffer, const unsigned int max_message_size);
enum NetworkCodes network_get_client_ip(int* socket, char *buffer);

#endif