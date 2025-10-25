# Network
## About 
It is simple internet function to work with sending data via network. It is written in C. Library is made for dynamic working it not waiting for read and send input between server and client.




## TOC
- [About](#about)
- [TOC](#toc)
- [Installation](#installation)
- [Usage](#usage)
- [Full Example](#full-example)
- [Feature](#feature)
- [License](#license)




## Installation
### As Lib
1. clone repository
```bash
  git clone https://github.com/Daynlight/Network.git
```
2. add subdirectory
```cmake
  add_subdirectory(Network)
```
3. add to your project
```cmake
  target_link_libraries(App Network)
```
### Chat Example
1. clone repository
```bash
  git clone https://github.com/Daynlight/Network.git
```
2. compile via cmake
```bash
  mkdir build
  cd build/
  cmake ..
```
3. run server
```bash
  ./../bin/Server
```
4. run client
```bash
  ./../bin/Client
```
5. (optional) run bash script to open server and two clients
```bash
  chmod +x ../.github/run_both.sh
  ./../.github/run_both.sh
```




## Usage
### Server
#### Functions
- ```network_server_init```
- ```network_server_destroy```
- ```network_server_listen```
- ```network_server_find_free_socket```
- ```network_server_read```
- ```network_server_send```
- ```network_server_broadcast```

#### Example
```c
#include "server.h"

#define PORT 9090
#define BUFFER_SIZE 100

struct network_server network = {0};
char buffer[BUFFER_SIZE] = {0};
int running = 1;

if (network_server_init(&network, PORT) == ERROR)
    exit(EXIT_FAILURE);

while (running) {
    if (network_server_listen(&network) == ERROR)
        printf("Can't connect client!\n");
    else
        printf("Client connected\n");

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (network.client_sock[i] > 0) {
            int valread = network_server_read(&network, i, buffer, BUFFER_SIZE);
            if (valread > 0) {
                printf("%s\n", buffer);
                network_server_broadcast(&network, i, buffer, BUFFER_SIZE);
            } else if (valread == DISCONNECT) {
                close(network.client_sock[i]);
                network.client_sock[i] = 0;
                printf("Client disconnected\n");
            }
        }
    }
}

network_server_destroy(&network);
```

### Client
#### Functions
- ```network_client_init```
- ```network_client_destroy```
- ```network_client_connect```
- ```network_client_read```
- ```network_client_send```

#### Example
```c
#include "client.h"

#define PORT 9090
#define IP "127.0.0.1"
#define BUFFER_SIZE 100

struct network_client network = {0};
char buffer[BUFFER_SIZE] = {0};
int running = 1;

if (network_client_init(&network, IP, PORT) == ERROR)
    printf("Can't init network\n");

if (network_client_connect(&network) == ERROR) {
    printf("Can't connect to network!\n");
    exit(EXIT_FAILURE);
} else {
    printf("Connected to server!\n");
}

while (running) {
    // Send data
    fgets(buffer, BUFFER_SIZE, stdin);
    if (strcmp(buffer, "exit") == 0) {
        running = 0;
        break;
    }

    network_client_send(&network, buffer, BUFFER_SIZE);

    // Receive data
    int valread = network_client_read(&network, buffer, BUFFER_SIZE);
    if (valread == DISCONNECT) {
        printf("Server closed the connection.\n");
        running = 0;
    } else if (valread > 0) {
        printf("Server: %s\n", buffer);
    }
}

network_client_destroy(&network);
```




## Full Example
### Server
```c
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
```

### Client
```c
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
    network_client_destroy(&network);
    running = 0;
    exit(EXIT_FAILURE);
};


int main(){
    signal(SIGINT, sigint_handler);

    if(network_client_init(&network, IP, PORT) == ERROR)
        printf("Can't init network\n");

    if(network_client_connect(&network) == ERROR){
        printf("Can't connect to network!\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("Connected to server!\n");

    printf("name: ");
    fgets(name, NAMESIZE -1, stdin);
    name[strcspn(name, "\n")] = ':';
    strcat(name, " ");
    printf("logged as %s\n", name);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    



    printf("> ");
    while (running) {
        if(fgets(send_buffer, BUFFER_SIZE, stdin) != NULL){
            send_buffer[strcspn(send_buffer, "\n")] = 0;

            if(strcmp(send_buffer, "exit") == 0){
                running = 0;
                break;
            };

            printf("> ");

            char buffer[NAMESIZE + BUFFER_SIZE] = {0};
            strcat(buffer, name);
            strcat(buffer, send_buffer);

            network_client_send(&network, buffer, NAMESIZE + BUFFER_SIZE);
            
            memset(buffer, 0, BUFFER_SIZE + NAMESIZE);
            memset(send_buffer, 0, BUFFER_SIZE);
        }
        else{
            int valread = network_client_read(&network, read_buffer, BUFFER_SIZE + NAMESIZE);
            if (valread == DISCONNECT) {
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
    


    network_client_destroy(&network);
    return 0;
};
```





## Features
- **Non-blocking I/O**: No need to wait for data to be read or sent; the program can continue executing while awaiting network events.
- **Multiple Client Handling**: The server can handle multiple client connections simultaneously.
- **Supports Broadcast**: The server can broadcast messages to all connected clients.
- **Extensible**: Future additions like two-way encryption (planned) or message queuing are easy to implement.

## TODO (Future Features)
- **Two-way** encryption (in progress)
- **Optimized** client/server performance for larger networks
- **Cross-Platform**: Should work on most platforms that support C, including Linux and Windows.





## License
[GNU GENERAL PUBLIC LICENSE Version 2, June 1991](LICENSE)




## Prerequisites
- CMake 3.15 or higher
- Git (for cloning with submodules)
