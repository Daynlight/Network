#include "server.h"

#define PORT 9090
#define BUFFER_SIZE 100
#define NAMESIZE 25

struct network_server network = {0};
char buffer[BUFFER_SIZE + NAMESIZE] = {0};
int running = 1;




void sigint_handler(int sig) {
    network_destroy_server(&network);
    running = 0;
    exit(EXIT_FAILURE);
};





int main() {
    signal(SIGINT, sigint_handler);

    printf("Server is running\n");

    if(network_init_server(&network, PORT) == -1){
        exit(EXIT_FAILURE);
    }

    
    while (running) {
        int err = network_listen(&network);
        if(err == -1){
            printf("Cant connect client!\n");
        }
        else if(err == 0){
            printf("Client connected\n");
        }

        for(int i = 0; i < MAX_CLIENTS; i++){
            memset(buffer, 0, BUFFER_SIZE + NAMESIZE);
            if(network.client_sock[i] > 0){
                int valread = network_read_server(&network, i, buffer,  BUFFER_SIZE + NAMESIZE);
                if(valread > 0){
                    printf("%s\n", buffer);
                    network_broadcast(&network, i, buffer, BUFFER_SIZE + NAMESIZE);
                }
                else if(valread == -2){
                    close(network.client_sock[i]);
                    network.client_sock[i] = 0;
                    printf("Client disconnected\n");
                };
            };
        };
    };

    network_destroy_server(&network);

    return 0;
}
