#include "server.h"

#define PORT 9090
#define BUFFER_SIZE 100
#define NAMESIZE 25

struct network_server network = {0};
char buffer[BUFFER_SIZE + NAMESIZE] = {0};
int running = 1;




void sigint_handler(int sig) {
    network_server_destroy(&network);
    running = 0;
    exit(EXIT_FAILURE);
};





int main() {
    signal(SIGINT, sigint_handler);

    printf("Server is running\n");

    if(network_server_init(&network, PORT) == ERROR)
        exit(EXIT_FAILURE);

    
    while (running) {
        // connect 
        enum NetworkCodes err = network_server_listen(&network);
        if(err == ERROR)
            printf("Cant connect client!\n");
        else if(err == SUCCESS)
            printf("Client connected\n");

        // broadcast
        for(int i = 0; i < MAX_CLIENTS; i++){
            memset(buffer, 0, BUFFER_SIZE + NAMESIZE);
            if(network.client_sock[i] > 0){
                int valread = network_server_read(&network, i, buffer,  BUFFER_SIZE + NAMESIZE);
                if(valread > 0){
                    printf("%s\n", buffer);
                    network_server_broadcast(&network, i, buffer, BUFFER_SIZE + NAMESIZE);
                }
                else if(valread == DISCONNECT){
                    close(network.client_sock[i]);
                    network.client_sock[i] = 0;
                    printf("Client disconnected\n");
                };
            };
        };
    };

    network_server_destroy(&network);

    return 0;
}
