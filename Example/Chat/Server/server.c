#include "server.h"


//   REGISTER = "1",         // 1<Name>
//   SENDMESSAGE = "2",      // 2<Message>
//   PRIVATEMESSAGE = "3"    // 3@<User>@<Message>



struct network_provider network = {0};
struct clients clients = {0};
struct clientData clientData = {0};
char buffer[BUFFER_SIZE + NAMESIZE] = {0};
int running = 1;





void sigint_handler(int sig) {
  destroy_clients(&clients);
  network_server_destroy(&network);
  printf("Clients destroyed!\n");
  printf("Server destroyed!\n");
  running = 0;
  exit(EXIT_FAILURE);
};





#ifdef WIN32
void client_sleep(float time_seconds){
  Sleep(time_seconds * 1000);
};
#else
void client_sleep(float time_seconds){
  sleep(time_seconds);
};
#endif






int main() {
  signal(SIGINT, sigint_handler);

  switch (network_server_init(&network, PORT)){
    case ERRORCODE:
      printf("Can't Initialize Server");
      network_server_destroy(&network);
      exit(EXIT_FAILURE);
      break;
    default: 
      printf("Server Initialized\n");
      printf("Server is running\n");
      break;
  };

  init_clients(&clients);

  while (running) {
    // connect
    int socket = network_server_listen(&network); 
    switch (socket){
      case ERRORCODE:
      printf("Cant connect client!\n");
      break;
    case NOCLIENT:
      break;
    default:
      char ip[INET_ADDRSTRLEN];
      network_get_client_ip(&socket, ip);
      add_client(&clients, socket);
      printf("Client connected ip: %s\n", ip);
      break;
    };


    for(int i = 0; i < clients.max_clients; i++){
      memset(buffer, 0, BUFFER_SIZE + NAMESIZE);
      if(clients.clientData[i].socket_id > 0){

        int valread = network_server_read(&clients.clientData[i].socket_id, buffer, BUFFER_SIZE + NAMESIZE);
        char decompressed_data[BUFFER_SIZE + NAMESIZE];
        compression_rle_decompress(buffer, decompressed_data);
      
        switch(valread){
        case DISCONNECT:
          char data[BUFFER_SIZE + NAMESIZE] = {0};
          strcat(data, "Client ");
          strcat(data, clients.clientData[i].name);
          strcat(data, " disconnected");
          char compressed_data[BUFFER_SIZE + NAMESIZE] = {};
          compression_rle_compress(data, compressed_data);
          
          printf("%s\n", data);
          for(int j = 0; j < clients.last_client; j++)
            if(i != j)
              network_server_send(&(clients.clientData[j].socket_id), compressed_data, BUFFER_SIZE + NAMESIZE);
          
          delete_client(&clients, i);
          
          break;
        case NODATA:
          break;
        default:
        
          if(decompressed_data[0] == '1'){       // Register
            char name[NAMESIZE];
            strcpy(name, decompressed_data + 1);
            strcpy(clients.clientData[i].name, name);
            printf("user %s registered\n", clients.clientData[i].name);
          }
          else if(decompressed_data[0] == '2'){  // Message
            char data[NAMESIZE + BUFFER_SIZE] = {0};
            strcat(data, clients.clientData[i].name);
            strcat(data, ": ");
            strcat(data, decompressed_data + 1);
            printf("public message: %s\n", data);
            char compressed_data[NAMESIZE + BUFFER_SIZE];
            compression_rle_compress(data, compressed_data);
            for(int j = 0; j < clients.last_client; j++)
              if(i != j && clients.clientData[j].socket_id > 0)
                network_server_send(&(clients.clientData[j].socket_id), compressed_data, BUFFER_SIZE + NAMESIZE);
          }
          else if(decompressed_data[0] == '3'){  // Private Message
            char data[NAMESIZE + BUFFER_SIZE] = {0};
            
            // get user name
            char* first = strchr(decompressed_data, '@'); 
            char* last = strchr(decompressed_data + 2, '@');
            int length = last - first;
            char target_name[NAMESIZE + BUFFER_SIZE] = {0};
            strncpy(target_name, decompressed_data + 2, length - 1);
            
            // make message
            strcat(data, clients.clientData[i].name);
            strcat(data, " <priv>: ");
            strcat(data, decompressed_data + length + 2);
            char compressed_data[NAMESIZE + BUFFER_SIZE];

            compression_rle_compress(data, compressed_data);
            
            printf("private message to %s: %s\n", target_name, data);

            // send to all
            for(int j = 0; j < clients.last_client; j++)
              if(i != j && clients.clientData[j].socket_id > 0 && strcmp(clients.clientData[j].name, target_name) == 0)
                network_server_send(&(clients.clientData[j].socket_id), compressed_data, BUFFER_SIZE + NAMESIZE);
          };
        break;
        };
      };
    };
    client_sleep(0.05);
  };

  destroy_clients(&clients);
  network_server_destroy(&network);
  printf("Clients destroyed!\n");
  printf("Server destroyed!\n");

  return 0;
};
