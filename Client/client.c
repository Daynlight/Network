#include "client.h"

#define PORT 9090
#define IP "127.0.0.1"

#define BUFFER_SIZE 100
#define NAMESIZE 25

struct network_client network = {0};
char send_buffer[BUFFER_SIZE] = {0};
char read_buffer[BUFFER_SIZE + NAMESIZE] = {0};
char name[NAMESIZE] = {0};
int running = 1;





void sigint_handler(int sig){
    network_destroy(&network);
    running = 0;
    exit(EXIT_FAILURE);
};






int main(){
    signal(SIGINT, sigint_handler);

    if(network_init(&network, IP, PORT)){
        printf("Can't init network\n");
    }

    if(network_connect(&network)){
        printf("Can't connect to network!\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Connected to server!\n");
    }  

    printf("name: ");
    fgets(name, NAMESIZE, stdin);
    name[strcspn(name, "\n")] = ' ';
    printf("logged as %s\n", name);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    


    printf("> ");
    while (running) {
        if(fgets(send_buffer, BUFFER_SIZE, stdin) != NULL){
            send_buffer[strcspn(send_buffer, "\n")] = 0;
            printf("> ");

            char buffer[NAMESIZE + BUFFER_SIZE] = {0};
            strcat(buffer, name);
            strcat(buffer, send_buffer);

            network_send(&network, buffer, NAMESIZE + BUFFER_SIZE);
            
            memset(buffer, 0, BUFFER_SIZE + NAMESIZE);
            memset(send_buffer, 0, BUFFER_SIZE);
        }
        else{
            int valread = network_read(&network, read_buffer, BUFFER_SIZE + NAMESIZE);
            if (valread < 0) {
                printf("\nServer closed the connection.\n");
                running = 0;
            }
            else if(valread > 0){
                printf("%s\n", read_buffer);
                printf("> ");
                memset(read_buffer, 0, BUFFER_SIZE + NAMESIZE);
            };
        };
    };
    


    network_destroy(&network);
    return 0;
};

