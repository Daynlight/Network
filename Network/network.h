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
#ifndef MAX_CLIENTS
#define MAX_CLIENTS 50
#endif




enum NetworkCodes{
  NOFREESOCKET = -4,
  NODATA = -3,
  DISCONNECT = -2,
  ERRORCODE = -1,
  NOCLIENT = -3,
  SUCCESS = 0
};




struct network_server{
  int server_sock;
  int client_sock[MAX_CLIENTS];
  struct sockaddr_in serv_addr;
};
struct network_client{
  int sock;
  struct sockaddr_in serv_addr;
};




//////////////////////////////////////////////////////////////////
///////////////////////////// Client /////////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_client_init(struct network_client* network_socket, const char* addr, int port);
enum NetworkCodes network_client_destroy(struct network_client* network_socket);
enum NetworkCodes network_client_connect(struct network_client* network_socket);
int network_client_read(struct network_client* network_socket, char* buffer, const unsigned int buffer_size);
enum NetworkCodes network_client_send(struct network_client* network_socket, char* buffer, const unsigned int max_message_size);




//////////////////////////////////////////////////////////////////
///////////////////////////// Server /////////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_server_init(struct network_server* network_socket, const unsigned int port);
enum NetworkCodes network_server_destroy(struct network_server* network_socket);
int network_server_listen(struct network_server* network_socket);
int network_server_find_free_socket(struct network_server* network_socket);
int network_server_read(struct network_server* network_socket, unsigned int i, char* buffer, const unsigned int buffer_size);
enum NetworkCodes network_server_send(struct network_server* network_socket, unsigned int i, char* buffer, const unsigned int max_message_size);
enum NetworkCodes network_server_broadcast(struct network_server *network_socket, const unsigned int i, char *buffer, const unsigned int max_message_size);
enum NetworkCodes network_server_get_client_ip(struct network_server *network_socket, const unsigned int i, char *buffer);


#endif