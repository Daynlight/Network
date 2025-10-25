#include "client.h"

#include <unistd.h>
#include <stdio.h>          // For standard input/output functions (printf, fgets, etc.)
#include <stdlib.h>         // For general utilities (exit, etc.)
#include <string.h>         // For memory and string functions (memset, strlen, etc.)
#include <unistd.h>         // For close(), read(), write(), etc.
#include <arpa/inet.h>      // For socket programming functions and structures (sockaddr_in, inet_pton, etc.)

#define PORT 9090           // The port number that the client will connect to
#define BUFFER_SIZE 50    // The size of the message buffer for sending/receiving data


int main(){
    client();
    return 0;
}




struct network{
    int sock;
    struct sockaddr_in serv_addr;
};

void initNetwork(struct network* network_socket, const char* ip_addr){
    // Create socket (IPv4, TCP)
    if ((network_socket->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");  // Error message if socket creation fails
        exit(-1);                              // Return -1 to indicate failure
    }

    network_socket->serv_addr.sin_family = AF_INET;             // Use IPv4 address family
    network_socket->serv_addr.sin_port = htons(PORT);           // Convert the port number to network byte 

    // Convert textual IP address ("127.0.0.1") to binary form and store in serv_addr.sin_addr
    // inet_pton() returns 1 if successful, 0 if invalid format, -1 if error
    if (inet_pton(AF_INET, ip_addr, &network_socket->serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n"); // Error if IP address is invalid
        exit(-1);                              // Return -1 to indicate failure
    }
};

void destroyNetwork(struct network* network_socket){
    close(network_socket->sock);                                // Close the socket after communication ends
};


void connectNetwork(struct network* network_socket){
    // Connect the socket to the specified server address
    while (1)
    {
        if (connect(network_socket->sock, (struct sockaddr *)&network_socket->serv_addr, sizeof(network_socket->serv_addr)) < 0) {
            sleep(10);
        }
        else{
            break;
        }
    }
    
}







int client() {
    struct network server = {0, 0};
    char buffer[BUFFER_SIZE] = {0};             // Buffer for storing data to send or receive

    initNetwork(&server, "127.0.0.1");

    connectNetwork(&server);

    printf("Connected to server!\n");           // Inform user that connection was successful

    // Continuous communication loop
    while (1) {
        printf("Client: ");                     // Prompt user for input
        fgets(buffer, BUFFER_SIZE, stdin);      // Read message from keyboard into buffer
        if(strlen(buffer) > BUFFER_SIZE) 
            send(server.sock, buffer, BUFFER_SIZE, 0);  // Send user message to the server
        else
            send(server.sock, buffer, strlen(buffer), 0);

        memset(buffer, 0, BUFFER_SIZE);         // Clear buffer before reading the server’s reply
        int valread = read(server.sock, buffer, BUFFER_SIZE); // Read message from server
        if (valread <= 0) {                     // If no data received or server closed connection
            printf("Server closed the connection.\n");
            break;                              // Exit communication loop
        }

        printf("Server: %s\n", buffer);         // Display message received from server
    }

    destroyNetwork(&server);
    return 0;                                   // Normal program termination
}
