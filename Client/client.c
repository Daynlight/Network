#include "client.h"

#define PORT 9090
#define BUFFER_SIZE 50


struct network_client network = {0, 0};


void sigint_handler(int sig){
    network_destroy(&network);
    exit(EXIT_FAILURE);
};


int main(){
    signal(SIGINT, sigint_handler);
    

    char buffer[BUFFER_SIZE] = {0};

    if(network_init(&network, "127.0.0.1", PORT))
        printf("Can't init network");

    if(network_connect(&network)){
        printf("Can't connect to network!\n");
        exit(-1);
    }
    else 
        printf("Connected to server!\n");


        
    while (1) {
        printf("Client: ");                     // Prompt user for input
        fgets(buffer, BUFFER_SIZE, stdin);      // Read message from keyboard into buffer
        if(strlen(buffer) > BUFFER_SIZE) 
            send(network.sock, buffer, BUFFER_SIZE, 0);  // Send user message to the server
        else
            send(network.sock, buffer, strlen(buffer), 0);

        memset(buffer, 0, BUFFER_SIZE);         // Clear buffer before reading the server’s reply
        int valread = read(network.sock, buffer, BUFFER_SIZE); // Read message from server
        if (valread <= 0) {                     // If no data received or server closed connection
            printf("Server closed the connection.\n");
            break;                              // Exit communication loop
        }

        printf("Server: %s\n", buffer);         // Display message received from server
    }

    network_destroy(&network);
    return 0;
};
