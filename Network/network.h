//////////////////////////////////////////////////////////////////
////////////////////////////// IDEA //////////////////////////////
//////////////////////////////////////////////////////////////////
// Server and Client works on no blocking sockets
// It is possibility that you don't receive data when read is called
// You may need make it on thread for continuous listing
// Or in while loop soo if you dont read in this time next time you will

// It is for simple data transport between server and client 

// For Video/Music etc. data are sended as buffer and 
// on client it is used like you want

// When used the best option is to make some custom format
// for example make first byte 255 op as operation
// 1 for register
// 2 for login 
// Then data you send
// Server read it and use it expected to operation

// For memory optimization best is used union and cast it to char
// Then send it and when reded cast it back
// Instead of sending string with data you send only data in binary
// Less size, safer, probably faster

// On your server you have to store client sockets and operate on them
// Make correct operation respected to given client request 

// After client connect it sets his socket to no-blocking



//////////////////////////////////////////////////////////////////
///////////////////////////// Client /////////////////////////////
//////////////////////////////////////////////////////////////////
// network_client_init      O(1) Just initialize client (Constructor)
// network_client_destroy   O(1) Just destroy client    (Destructor)
// network_client_connect   O(n) MAX_CONNECT_RETRYS * CONNECT_RETRY_DELAY for each but it's ok we want it
// network_client_read      O(n) depends on data size and buffer we used  ->  operations = data / buffer size
// network_client_send      O(n) depends on data size and buffer we used  ->  operations = data / buffer size



//////////////////////////////////////////////////////////////////
///////////////////////////// Server /////////////////////////////
//////////////////////////////////////////////////////////////////
// network_server_init      O(1) Just initialize server (Constructor)
// network_server_destroy   O(1) Just destroy server    (Destructor)
// network_server_listen    O(1) Listen if any of users is tring to connect and accept it
// network_server_read      O(n) depends on data size and buffer we used  ->  operations = data / buffer size
// network_server_send      O(n) depends on data size and buffer we used  ->  operations = data / buffer size
// network_server_broadcast O(n) broad cast buffer to every client without i(sending if -1 send to all)
// network_get_client_ip    O(1) 



///////////////////////////////////////////////////////////////////
////////////////////////////// Codes //////////////////////////////
///////////////////////////////////////////////////////////////////
// NODATA       send or read no data
// DISCONNECT   server or client is disconnecting
// NOCLIENT     no client to connect
// DNSERROR     can't find domain
// SOCKETERROR  can't create socket
// CONNECTERROR can't connect to server
// ERRORCODE    error when function called
// SUCCESS      fuction end up with success



//////////////////////////////////////////////////////////////////
///////////////////////// Planed Features ////////////////////////
//////////////////////////////////////////////////////////////////
// TCP/UDP protocols                (set in server and in client automatically getted from server on connect if possible)
// Better error checking
// Highly optimized client system   (no verdict because it is highly dependent from server implementation if it would be c++ I would use templates but it is c :<)
// Buffer to way encryption x25519  (functions that generate buffer from buffer but encrypted/decrypted)
// Buffer compression LZ4           (functions that generate buffer from buffer but compressed/uncompressed)


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



struct network_provider{
  int sock;
  struct sockaddr_in serv_addr;
};



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



//////////////////////////////////////////////////////////////////
///////////////////////// Server/Client //////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_check_if_empty_message(char *buffer);


#endif