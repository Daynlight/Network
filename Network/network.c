#include "network.h"




//////////////////////////////////////////////////////////////////
///////////////////////////// Client /////////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_client_init(struct network_client* network_socket, const char* addr, int port) {
#ifdef WIN32
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  struct addrinfo hints, *res;
  int err_code;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if ((err_code = getaddrinfo(addr, NULL, &hints, &res)) != 0)
    return ERRORCODE;

  if ((network_socket->sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
    freeaddrinfo(res);
    return ERRORCODE;
  }

  network_socket->serv_addr = *(struct sockaddr_in*)res->ai_addr;
  network_socket->serv_addr.sin_port = htons(port);

  freeaddrinfo(res);

#ifndef WIN32
  int flags = fcntl(network_socket->sock, F_GETFL, 0);
  fcntl(network_socket->sock, F_SETFL, flags | O_NONBLOCK);
#endif

  return SUCCESS;
}


enum NetworkCodes network_client_destroy(struct network_client* network_socket){
  if(network_socket->sock != -1)
#ifdef WIN32
    closesocket(network_socket->sock);
#else
    close(network_socket->sock);
#endif

#ifdef WIN32
  WSACleanup();
#endif

  return SUCCESS;
};

enum NetworkCodes network_client_connect(struct network_client* network_socket) {
#ifdef WIN32
  if (connect(network_socket->sock, (struct sockaddr *)&network_socket->serv_addr, sizeof(network_socket->serv_addr)) < 0) {
    int err = WSAGetLastError();
    if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS) {
      return ERRORCODE;
    };
 
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(network_socket->sock, &writefds);

    struct timeval tv;
    tv.tv_sec = CONNECT_RETRY_DELAY;
    tv.tv_usec = 0;

    int sel = select(0, NULL, &writefds, NULL, &tv);
    if (sel > 0 && FD_ISSET(network_socket->sock, &writefds)) {
      return SUCCESS;
    } else {
      return ERRORCODE;
    }
  } else {
    u_long mode = 1;
    ioctlsocket(network_socket->sock, FIONBIO, &mode);
    return SUCCESS;
  };
#else
  unsigned int retry = 0;
  while (retry <= MAX_CONNECT_RETRYS) {
    if (connect(network_socket->sock, (struct sockaddr *)&network_socket->serv_addr, sizeof(network_socket->serv_addr)) < 0) {
      retry++;
      sleep(CONNECT_RETRY_DELAY);
    } 
    else
      return SUCCESS;
    };
  return ERRORCODE;
#endif
};

int network_client_read(struct network_client *network_socket, char *buffer, const unsigned int buffer_size) {
#ifdef WIN32
  int val = recv(network_socket->sock, buffer, buffer_size, 0);
  if(val == SOCKET_ERROR){
    int err = WSAGetLastError();
    if(err == WSAEWOULDBLOCK)
      return NODATA;
    else
      return ERRORCODE;
  };
#else
  int val = read(network_socket->sock, buffer, buffer_size);
  if(val < 0){
    if(errno == EAGAIN || errno == EWOULDBLOCK)
      return NODATA;
    else
      return ERRORCODE;
  };
#endif

  if(val == 0)
    return DISCONNECT;

  return val;
};

enum NetworkCodes network_client_send(struct network_client *network_socket, char *buffer, const unsigned int max_message_size){
  if(buffer == NULL || strlen(buffer) == 0)
    return NODATA;

  if(strlen(buffer) > max_message_size)
    send(network_socket->sock, buffer, max_message_size, 0);
  else
    send(network_socket->sock, buffer, strlen(buffer), 0);
    
  return SUCCESS;
};





//////////////////////////////////////////////////////////////////
///////////////////////////// Server /////////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_server_init(struct network_server *network_socket, const unsigned int port){
#ifdef WIN32
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  int opt = 1;
  int addrlen = sizeof(network_socket->serv_addr);

  if ((network_socket->server_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    return ERRORCODE;

#ifdef WIN32
  if (setsockopt(network_socket->server_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)))
    return ERRORCODE;
#else  
  if (setsockopt(network_socket->server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    return ERRORCODE;
#endif

  network_socket->serv_addr.sin_family = AF_INET;
  network_socket->serv_addr.sin_addr.s_addr = INADDR_ANY;
  network_socket->serv_addr.sin_port = htons(port);

  if (bind(network_socket->server_sock, (struct sockaddr *)&network_socket->serv_addr, sizeof(network_socket->serv_addr)) < 0)
    return ERRORCODE;

#ifdef WIN32
  u_long mode = 1;
  ioctlsocket(network_socket->server_sock, FIONBIO, &mode);
#else
  int flags = fcntl(network_socket->server_sock, F_GETFL, 0);
  fcntl(network_socket->server_sock, F_SETFL, flags | O_NONBLOCK);
#endif

  return SUCCESS;
};

enum NetworkCodes network_server_destroy(struct network_server *network_socket) {
  if(network_socket->server_sock != -1)
#ifdef WIN32
    closesocket(network_socket->server_sock);
#else
    close(network_socket->server_sock);
#endif

#ifdef WIN32
  WSACleanup();
#endif

  return SUCCESS;
};

int network_server_listen(struct network_server *network_socket) {
  int addrlen = sizeof(network_socket->serv_addr);
    
  if (listen(network_socket->server_sock, 3) < 0)
    return ERRORCODE;

  int socket = accept(network_socket->server_sock, (struct sockaddr *)&network_socket->serv_addr, (socklen_t*)&addrlen);
  if (socket < 0)
    return NOCLIENT;

#ifdef WIN32
  u_long mode = 1;
  ioctlsocket(socket, FIONBIO, &mode);
#else
  int flags = fcntl(socket, F_GETFL, 0);
  fcntl(socket, F_SETFL, flags | O_NONBLOCK);
#endif

  return socket;
};

int network_server_read(int* socket, char *buffer, const unsigned int buffer_size){
#ifdef WIN32
  int val = recv(socket, buffer, buffer_size, 0);
  if(val == SOCKET_ERROR){
    int err = WSAGetLastError();
    if(err == WSAEWOULDBLOCK)
      return NODATA;
    else
      return ERRORCODE;
  };
#else
  int val = read(*socket, buffer, buffer_size);
  if(val < 0){
    if(errno == EAGAIN || errno == EWOULDBLOCK)
      return NODATA;
    else
      return ERRORCODE;
  };
#endif

  if(val == 0)
    return DISCONNECT;

  return val;
}

enum NetworkCodes network_server_send(int* socket, char *buffer, const unsigned int max_message_size) {
  if(strlen(buffer) == 0)
    return NODATA;

  if(strlen(buffer) > max_message_size)
    send(*socket, buffer, max_message_size, 0);
  else
    send(*socket, buffer, strlen(buffer), 0);
  
  return SUCCESS;
};

enum NetworkCodes network_server_broadcast(struct network_server *network_socket, int* clients_sockets, unsigned int max_clients, const unsigned int i, char *buffer, const unsigned int max_message_size) {
  for(int j = 0; j < max_clients; j++)
    if(i != j && clients_sockets[j] > 0)
      network_server_send(&(clients_sockets[j]), buffer,  max_message_size);

  return SUCCESS; 
}

enum NetworkCodes network_get_client_ip(int* socket, char *buffer){
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  char client_ip[INET_ADDRSTRLEN];
  if (getpeername(*socket, (struct sockaddr *)&client_addr, &client_len) != 0) {
    return ERRORCODE;
  }
  
  inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

  strncpy(buffer, client_ip, INET_ADDRSTRLEN - 1);
  buffer[INET_ADDRSTRLEN - 1] = '\0'; 
  
  return SUCCESS;
};