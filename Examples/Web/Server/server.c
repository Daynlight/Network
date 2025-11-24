#include "server.h"



struct network_provider network = {0};
struct clients clients = {0};
int running = 1;


int total = 0;
char last_path[255] = {0};

void sigint_handler(int sig) {
  destroy_clients(&clients);
  network_server_destroy(&network);
  print_and_log("Clients destroyed!\n");
  print_and_log("Server destroyed!\n");
  running = 0;
  exit(EXIT_FAILURE);
};


int main() {
  // SIGINT callback
  signal(SIGINT, sigint_handler);
  

  // Init server
  switch (network_server_init(&network, TCP, PORT)){
    case ERRORCODE:
      print_and_log("Can't Initialize Server\n");
      network_server_destroy(&network);
      perror("error");
      exit(EXIT_FAILURE);
      break;
    default: 
      print_and_log("Server Initialized\n");
      print_and_log("Server is running\n");
      break;
  };

  // Init clients
  init_clients(&clients);

  char respond_message[BUFFER_SIZE] = {0};
  FILE *fptr = fopen("index.html", "r");
  char ch;
  int i = 0;
  while ((ch = fgetc(fptr)) != EOF){
    respond_message[i] = ch;
    i++;
  };




  while (running) {

    time_t currentTime;
    time(&currentTime);
    char end_message[BUFFER_SIZE] = {0};  //// syf af
    strcat(end_message, respond_message);
    strcat(end_message, ctime(&currentTime));


    // listen for new connections
    listen_for_connections(&network, &clients);

    // responds
    for(int i = 0; i < clients.last_client; i++){
      char buffer[BUFFER_SIZE + NAMESIZE] = {0};

      if(clients.clientData[i].socket_id > 0){

        // read requests
        int valread = network_server_read(&clients.clientData[i].socket_id, buffer, BUFFER_SIZE + NAMESIZE);
      
        buffer[strlen(buffer)] = '\0';
        print_and_log("%s\n", buffer);

        char path[255] = {0};
        get_path(buffer, path);

        print_and_log("%s\n", path);

        strcat(end_message, path);
        
        strcat(end_message, "<h1>My HTTP server statistics</h1> <p>Total requests: ");
        char total_str[16];
        sprintf(total_str, "%d", total / 2);
        strcat(end_message, total_str);
        strcat(end_message, "</p>");

        strcat(end_message, "<p>Last requested file: ");
        strcat(end_message, last_path);
        strcat(end_message, "</p>");
        if(total % 2 == 0)
          strcpy(last_path, path);

        // respond to request
        switch(valread){
          case DISCONNECT:
            disconnect_user(&clients, i);
            break;
          case NODATA:
            break;
          default:
            respond(end_message, &clients, i);
            disconnect_user(&clients, i);
            total++;
          break;
        };

      };
    };

#ifdef WIN32
    Sleep(0.05);
#else
    sleep(0.05);
#endif
  };

  // clean up
  destroy_clients(&clients);
  network_server_destroy(&network);
  print_and_log("Clients destroyed!\n");
  print_and_log("Server destroyed!\n");

  return 0;
};
