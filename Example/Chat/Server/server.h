#ifndef SERVER_H
#define SERVER_H

#include "network.h"
#include <signal.h>

#ifdef WIN32
  #include <windows.h>
#endif



struct clients{
  int* client_sock;
  unsigned int max_clients;
  unsigned int last_client;
};



#endif