#ifndef NETWORK_H
#define NETWORK_H



#ifdef WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <unistd.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <errno.h>
  #include <netdb.h> 
  #include <arpa/inet.h>
#endif




///////////////////////////////////////////////////////////////////
////////////////////////////// Codes //////////////////////////////
///////////////////////////////////////////////////////////////////
enum NetworkCodes{
  CONNECTERROR = -7,
  SOCKETERROR = -6,
  DNSERROR = -5,
  NODATA = -4,
  NOCLIENT = -3,
  DISCONNECT = -2,
  ERRORCODE = -1,
  SUCCESS = 0
};

///////////////////////////////////////////////////////////////////
////////////////////////////// Modes //////////////////////////////
///////////////////////////////////////////////////////////////////
enum NetworkModes{
  TCP = 0,
  UDP = 1
};



struct network_provider{
  int sock;
  struct sockaddr_in serv_addr;
  enum NetworkModes mode;
};


//////////////////////////////////////////////////////////////////
///////////////////////////// Client /////////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_client_init(struct network_provider* network_provider, enum NetworkModes mode, const char* addr, int port);
enum NetworkCodes network_client_destroy(struct network_provider* network_provider);
enum NetworkCodes network_client_connect(struct network_provider* network_provider);
int network_client_read(struct network_provider* network_provider, char* buffer, const unsigned int buffer_size);
enum NetworkCodes network_client_send(struct network_provider* network_provider, char* buffer, const unsigned int max_message_size);
int network_client_read_from(struct network_provider* network_provider, char* buffer, const unsigned int buffer_size);
enum NetworkCodes network_client_send_to(struct network_provider* network_provider, char* buffer, const unsigned int max_message_size);
int network_client_send_request(struct network_provider* network_provider, char* request, const unsigned int max_message_size,
                                char* respond, float refresh_rate, unsigned int max_tries);


//////////////////////////////////////////////////////////////////
///////////////////////////// Server /////////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_server_init(struct network_provider* network_provider, enum NetworkModes mode, const unsigned int port);
enum NetworkCodes network_server_destroy(struct network_provider* network_provider);
int network_server_listen(struct network_provider* network_provider);
int network_server_read(int* socket, char *buffer, const unsigned int buffer_size);
enum NetworkCodes network_server_send(int* socket, char *buffer, const unsigned int max_message_size);
enum NetworkCodes network_get_client_ip(int* socket, char *buffer); 
int network_server_read_from(struct network_provider* network_provider, struct sockaddr *address, char* buffer, const unsigned int buffer_size);
enum NetworkCodes network_server_send_to(struct network_provider *network_provider, struct sockaddr *dest, char *buffer, const unsigned int max_message_size);


//////////////////////////////////////////////////////////////////
///////////////////////// Server/Client //////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_check_if_empty_message(char *buffer);


#endif