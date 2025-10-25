#include "network.h"



void network_destroy(struct network_client* network_socket){
  if(network_socket->sock != -1)
    close(network_socket->sock);
}

int network_connect(struct network_client* network_socket){
  unsigned int retry = 0;

  while (retry <= MAX_CONNECT_RETRYS) {
    if (connect(network_socket->sock, (struct sockaddr *)&network_socket->serv_addr, sizeof(network_socket->serv_addr)) < 0){
      retry++;
      sleep(CONNECT_RETRY_DELAY);
    }  
    else{
      return 0;
    };
  };

  return -1;
};

int network_init(struct network_client* network_socket, const char* ip_addr, int port){
    if ((network_socket->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    network_socket->serv_addr.sin_family = AF_INET;
    network_socket->serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip_addr, &network_socket->serv_addr.sin_addr) <= 0)
        return -1;

    int flags = fcntl(network_socket->sock, F_GETFL, 0);
    fcntl(network_socket->sock, F_SETFL, flags | O_NONBLOCK);

    return 0;
}

int network_read(struct network_client *network_socket, char *buffer, const unsigned int buffer_size) {
    int val = read(network_socket->sock, buffer, buffer_size);

    if (val == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        } else {
            return -1;
        }
    }

    if(val == 0)
      return -2;

    return val;
}

void network_send(struct network_client *network_socket, char *buffer, const unsigned int max_message_size){
  if(strlen(buffer) > max_message_size)
      send(network_socket->sock, buffer, max_message_size, 0);
  else
      send(network_socket->sock, buffer, strlen(buffer), 0);
}






void network_destroy_server(struct network_server *network_socket) {
  for(int i = 0; i < MAX_CLIENTS; i++)
    if(network_socket->client_sock[i] != -1)
      close(network_socket->client_sock[i]);

  if(network_socket->server_sock != -1)
    close(network_socket->server_sock);
}

int network_find_free_socket(struct network_server *network_socket) {
  int socket = -1;
  for(int i = 0; i < MAX_CLIENTS; i++)
    if(network_socket->client_sock[i] == 0){
      socket = i;
      break;
    }

  return socket;
}

int network_listen(struct network_server *network_socket) {
  int addrlen = sizeof(network_socket->serv_addr);
    
  if (listen(network_socket->server_sock, 3) < 0){
    return -1;
  }

  int socket = accept(network_socket->server_sock, (struct sockaddr *)&network_socket->serv_addr, (socklen_t*)&addrlen);
  if (socket < 0) {
    return 0;
  }

  int socket_place = network_find_free_socket(network_socket);
  if(socket_place == -1){
    return -1;
  }
    
  network_socket->client_sock[socket_place] = socket;
  return 0;
}

int network_init_server(struct network_server *network_socket, const unsigned int port){
    int opt = 1;
    int addrlen = sizeof(network_socket->serv_addr);

    if ((network_socket->server_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      return -1;
    }

    if (setsockopt(network_socket->server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
      return -1;
    }

    network_socket->serv_addr.sin_family = AF_INET;
    network_socket->serv_addr.sin_addr.s_addr = INADDR_ANY;
    network_socket->serv_addr.sin_port = htons(port);

    if (bind(network_socket->server_sock, (struct sockaddr *)&network_socket->serv_addr, sizeof(network_socket->serv_addr)) < 0) {
      return -1;
    }

    int flags = fcntl(network_socket->server_sock, F_GETFL, 0);
    fcntl(network_socket->server_sock, F_SETFL, flags | O_NONBLOCK);
    for(int i = 0; i < MAX_CLIENTS; i++)
      fcntl(network_socket->client_sock[i], F_SETFL, flags | O_NONBLOCK);

    return 0;
}

int network_read_server(struct network_server *network_socket, unsigned int i, char *buffer, const unsigned int buffer_size){
    int val = read(network_socket->client_sock[i], buffer, buffer_size);

    if (val == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        } else {
            printf("Read error: %s\n", strerror(errno));
            return -1;
        }
    }

    if(val == 0)
      return -2;

    return val;
}

void network_send_server(struct network_server *network_socket, const unsigned int i, char *buffer, const unsigned int max_message_size) {
  if(strlen(buffer) > max_message_size)
      send(network_socket->client_sock[i], buffer, max_message_size, 0);
  else
      send(network_socket->client_sock[i], buffer, strlen(buffer), 0);
};
