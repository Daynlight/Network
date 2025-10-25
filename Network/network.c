#include "network.h"



void network_destroy(struct network_client* network_socket){
  if(network_socket->sock != -1)
    close(network_socket->sock);
};

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

    return 0;
};