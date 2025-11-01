#include "network.h"



//////////////////////////////////////////////////////////////////
///////////////////////////// Client /////////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_client_init(struct network_provider* network_provider, const char* addr, int port) {
#ifdef WIN32
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  struct addrinfo hints, *res;
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(addr, NULL, &hints, &res) != 0){
    freeaddrinfo(res);
    return DNSERROR;
  };
    

  if ((network_provider->sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
    freeaddrinfo(res);
    return SOCKETERROR;
  };

  network_provider->serv_addr = *(struct sockaddr_in*)res->ai_addr;
  network_provider->serv_addr.sin_port = htons(port);

  freeaddrinfo(res);

  return SUCCESS;
};




enum NetworkCodes network_client_destroy(struct network_provider* network_provider){
  if(network_provider->sock != -1)
#ifdef WIN32
    closesocket(network_provider->sock);
#else
    close(network_provider->sock);
#endif

#ifdef WIN32
  WSACleanup();
#endif

  return SUCCESS;
};




enum NetworkCodes network_client_connect(struct network_provider* network_provider) {
#ifdef WIN32
  if (connect(network_provider->sock, (struct sockaddr *)&network_provider->serv_addr, sizeof(network_provider->serv_addr)) < 0) {
    int err = WSAGetLastError();
    if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS) {
      return ERRORCODE;
    };
 
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(network_provider->sock, &writefds);

    struct timeval tv;
    tv.tv_sec = CONNECT_RETRY_DELAY;
    tv.tv_usec = 0;

    int sel = select(0, NULL, &writefds, NULL, &tv);
    if (sel > 0 && FD_ISSET(network_provider->sock, &writefds)) {
      return SUCCESS;
    } else {
      return ERRORCODE;
    }
  } else {
    u_long mode = 1;
    ioctlsocket(network_provider->sock, FIONBIO, &mode);
    return SUCCESS;
  };
#else
  if (connect(network_provider->sock, (struct sockaddr *)&network_provider->serv_addr, sizeof(network_provider->serv_addr)) >= 0) {
    int flags = fcntl(network_provider->sock, F_GETFL, 0);
    fcntl(network_provider->sock, F_SETFL, flags | O_NONBLOCK);
    return SUCCESS;
  };
  return CONNECTERROR;
#endif
};




int network_client_read(struct network_provider *network_provider, char *buffer, const unsigned int buffer_size) {
#ifdef WIN32
  int val = recv(network_provider->sock, buffer, buffer_size, 0);
  if(val == SOCKET_ERROR){
    int err = WSAGetLastError();
    if(err == WSAEWOULDBLOCK)
      return NODATA;
    else
      return ERRORCODE;
  };
#else
  int val = read(network_provider->sock, buffer, buffer_size);
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





enum NetworkCodes network_client_send(struct network_provider *network_provider, char *buffer, const unsigned int max_message_size){
  if(buffer == NULL || strlen(buffer) == 0)
    return NODATA;

  if(strlen(buffer) > max_message_size)
    send(network_provider->sock, buffer, max_message_size, 0);
  else
    send(network_provider->sock, buffer, strlen(buffer), 0);
    
  return SUCCESS;
};







//////////////////////////////////////////////////////////////////
///////////////////////////// Server /////////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_server_init(struct network_provider *network_provider, const unsigned int port){
#ifdef WIN32
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  int opt = 1;
  int addrlen = sizeof(network_provider->serv_addr);

  if ((network_provider->sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    return ERRORCODE;

#ifdef WIN32
  if (setsockopt(network_provider->sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)))
    return ERRORCODE;
#else  
  if (setsockopt(network_provider->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    return ERRORCODE;
#endif

  network_provider->serv_addr.sin_family = AF_INET;
  network_provider->serv_addr.sin_addr.s_addr = INADDR_ANY;
  network_provider->serv_addr.sin_port = htons(port);

  if (bind(network_provider->sock, (struct sockaddr *)&network_provider->serv_addr, sizeof(network_provider->serv_addr)) < 0)
    return ERRORCODE;

#ifdef WIN32
  u_long mode = 1;
  ioctlsocket(network_provider->sock, FIONBIO, &mode);
#else
  int flags = fcntl(network_provider->sock, F_GETFL, 0);
  fcntl(network_provider->sock, F_SETFL, flags | O_NONBLOCK);
#endif

  return SUCCESS;
};






enum NetworkCodes network_server_destroy(struct network_provider *network_provider) {
  if(network_provider->sock != -1)
#ifdef WIN32
    closesocket(network_provider->sock);
#else
    close(network_provider->sock);
#endif

#ifdef WIN32
  WSACleanup();
#endif

  return SUCCESS;
};

int network_server_listen(struct network_provider *network_provider) {
  int addrlen = sizeof(network_provider->serv_addr);
    
  if (listen(network_provider->sock, 3) < 0)
    return ERRORCODE;

  int socket = accept(network_provider->sock, (struct sockaddr *)&network_provider->serv_addr, (socklen_t*)&addrlen);
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
};




enum NetworkCodes network_server_send(int* socket, char *buffer, const unsigned int max_message_size) {
  if(strlen(buffer) == 0)
    return NODATA;

  if(strlen(buffer) > max_message_size)
    send(*socket, buffer, max_message_size, 0);
  else
    send(*socket, buffer, strlen(buffer), 0);
  
  return SUCCESS;
};





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






//////////////////////////////////////////////////////////////////
///////////////////////// Server/Client //////////////////////////
//////////////////////////////////////////////////////////////////
enum NetworkCodes network_check_if_empty_message(char *buffer){
  if(buffer == NULL || strlen(buffer) == 0)
    return NODATA;

  return SUCCESS;
};



