#include "client.h"

#define PORT 9090
#define BUFFER_SIZE 100
#define NAMESIZE 25


struct network_client network = {0, 0};
char send_buffer[BUFFER_SIZE] = {0};
char read_buffer[BUFFER_SIZE] = {0};
char name[NAMESIZE] = {0};
int running = 1;

int read_thread_running = 1;
pthread_t thread_id;
int thread_num = 1;



void sigint_handler(int sig){
    network_destroy(&network);
    running = 0;
    read_thread_running = 0;
    pthread_join(thread_id, NULL);
    exit(EXIT_FAILURE);
};


void* read_thread(){
    int valread = 0;
    while(read_thread_running){
        valread = network_read(&network, read_buffer, BUFFER_SIZE);
        if (valread < 0) {
            printf("\nServer closed the connection.\n");
            running = 0;
            read_thread_running = 0;
        }
        else if(valread > 0){
            printf("Server: %s", read_buffer);
            fflush(stdout);
            printf("> ");
            fflush(stdout);
        };
    };
    return NULL;
}


int main(){
    signal(SIGINT, sigint_handler);
    

    if(network_init(&network, "127.0.0.1", PORT))
        printf("Can't init network");

    if(network_connect(&network)){
        printf("Can't connect to network!\n");
        exit(-1);
    }
    else 
        printf("Connected to server!\n");

    if (pthread_create(&thread_id, NULL, read_thread, (void*)&thread_num) != 0) {
        perror("pthread_create");
        exit(1);
    }

    printf("name: ");
    fgets(name, NAMESIZE, stdin);
    name[strcspn(name, "\n")] = ' ';


    printf("> ");
    while (running) {
        fgets(send_buffer, BUFFER_SIZE, stdin);
        printf("> ");

        char buffer[NAMESIZE + BUFFER_SIZE] = {0};
        strcat(buffer, name);
        strcat(buffer, send_buffer);

        if(strlen(send_buffer) > BUFFER_SIZE)
            send(network.sock, buffer, BUFFER_SIZE + NAMESIZE, 0);
        else
            send(network.sock, buffer, strlen(buffer), 0);

        memset(buffer, 0, BUFFER_SIZE);
    };
    
    pthread_join(thread_id, NULL);
    network_destroy(&network);
    return 0;
};

