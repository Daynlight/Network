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
        if(network_listen(&network) == -1){
            printf("Cant connect client!\n");
        }

        for(int i = 0; i < MAX_CLIENTS; i++){
            memset(buffer, 0, BUFFER_SIZE + NAMESIZE);
            if(network.client_sock[i] > 0){
                int valread = network_read_server(&network, i, buffer,  BUFFER_SIZE + NAMESIZE);
                printf("message read from %d: %s", i, buffer);
                if(valread > 0){
                    for(int j = 0; j < MAX_CLIENTS; j++){
                        if(i != j && network.client_sock[i] > 0){
                            network_send_server(&network, j, buffer,  BUFFER_SIZE + NAMESIZE);
                            printf("%d to %d: %s\n", i, j, buffer);
                        };
                    };          
                };
            };
        };
    };

    network_destroy_server(&network);

    return 0;
}
