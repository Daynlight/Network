# Network
## About 
It is simple internet interface for easier working with networking in c/c++. The library is written in c. Useful and lightweight. Core idea was assignment at university with networking. All functions operate in **non-blocking mode**. Includes a simple chat example with a server, allowing many users to chat with each other.


<div align=center>

![Tools](https://go-skill-icons.vercel.app/api/icons?i=c,cmake,docker,git,github,githubactions,windows,linux)

<sub>Built with C · CMake · Docker · GitHub Actions · Cross-Platform</sub>

</div>



## TOC
- [About](#about)
- [TOC](#toc)
- [Screenshots](#screenshots)
- [Supported Platforms](#supported-platforms)
- [Installation](#installation)
- [Usage](#usage)
- [Code Snippets](#code-snippets)
- [Full Example](#full-example)
- [Features](#features)
- [License](#license)
- [Prerequisites](#prerequisites)




## Screenshots

<table>
  <tr>
    <td align="center">
      <a href=".github/SS//Chat.png">
        <img src=".github/SS//Chat.png" /><br>
      </a>
    </td>
  </tr>
</table>



## Supported Platforms
| Platform   | support |
|:----------:|:-------:|
| **Linux**  | ✅ Yes  |
| **Windows**| ✅ Yes  |
| **Android**| ❌ No   |
| **macOS**  | ❌ No   |
| **iOS**    | ❌ No   |



## Installation
### As Library
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
  # you need to change address in Example/Chat/Macro.h
  # in default it uses my google cloud server so many 
  # users can access it

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




## Usage
### Info and Recommend Usage
- Server and Client works on **non-blocking sockets**. You need loop for receiving data.

- For **Video/Music etc.** data are send as **char buffer** and on client side are used in proper way.

- Best way to use it is making some custom format. for example make first **byte 255 or more** are always operations **register,login etc.** then you check this first byte and make operation based on this.

- Good practice is to **optimize** buffer before sending it, **SIGINT handling**. Also good idea is **compression** and **encryption** [CCrypt](https://github.com/Daynlight/CCrypt).

- After client connect it sets his socket to **non-blocking**

### SIGINT handling
```c
void sigint_handler(int sig){
  network_client_destroy(&network);
  printf("Network destroyed!\n");
  running = 0;
  exit(EXIT_FAILURE);
};
```

### Server
#### Functions
- ```network_server_init``` - initialize server
- ```network_server_destroy``` - destroy server
- ```network_server_listen``` - tcp listen for users connections
- ```network_server_read``` - tcp read data from client
- ```network_server_send``` - tcp send data to client
- ```network_get_client_ip``` - get ip from socket
- ```network_server_read_from```  - udp read data from client
- ```network_server_send_to```  - tcp send data to client


### Client
#### Functions
- ```network_client_init``` - initialize client
- ```network_client_destroy```  - destroy client
- ```network_client_connect```  - tcp connect client to server
- ```network_client_read``` - tcp read data from server
- ```network_client_send``` - tcp send data to server
- ```network_client_read_from``` - udp read data from server  
- ```network_client_send_to```  - udp send data to server
- ```network_client_send_request``` - udp combination of send and read

### Server/Client
- ```network_check_if_empty_message``` - check if message is not empty

### NetworkModes
- ```TCP``` - tcp protocol
- ```UDP``` - udp protocol

### TCP Mode 
- In tcp mode client and server creates connection via ```listen``` and ```connect```
- ```listen``` returns user socket you should implement your own client manager
- Data are exchanged via ```send``` and ```read``` functions for ```client``` and ```server```
- You need specify maximal message size
- If message is to long then send up to max size

### UDP Mode
- In udp mode client and server don't create connection so you don't use ```listen``` and ```connect```
- You also don't need to create client manager
- Data are exchanged via ```send_to``` and ```read_from```
- For client You can run ```request``` that is combination of ```send_to``` and ```read_from```
- For server when ```read_from``` You add reference to client and then when ```send_to``` use it
- You need specify maximal message size
- If message is to long then send up to max size

### Network Provider
- ```sock``` - socket id
- ```serv_addr``` - sockaddr_in for server
- ```mode``` - contains TCP/UDP mode used in socket

### NetworkCodes
- ```NODATA``` - send or read no data
- ```DISCONNECT``` - server or client is disconnecting
- ```NOCLIENT``` - no client to connect
- ```DNSERROR``` - can't find domain
- ```SOCKETERROR``` - can't create socket
- ```CONNECTERROR```  can't connect to server
- ```ERRORCODE``` - error when function called
- ```SUCCESS``` - function end up with success



## Code Snippets
### Server
#### network_server_init
```c
  switch (network_server_init(&network, UDP, PORT)){
    case ERRORCODE:
      printf("Can't Initialize Server\n");
      network_server_destroy(&network);
      exit(EXIT_FAILURE);
      break;
    default: 
      printf("Server Initialized\n");
      printf("Server is running\n");
      break;
  };
```

#### network_server_destroy
```c
  network_server_destroy(&network);
```

#### network_server_listen
```c
  int socket = network_server_listen(network); 
```

#### network_server_read
```c
  char buffer[BUFFER_SIZE + NAMESIZE] = {0};
  
  int valread = network_server_read(&clients.clientData[i].socket_id, buffer, BUFFER_SIZE + NAMESIZE);
```

#### network_server_send
```c
  char message[NAMESIZE + BUFFER_SIZE] = "message";
  
  network_server_send(&(clients->clientData[j].socket_id), message, BUFFER_SIZE + NAMESIZE);
```

#### network_get_client_ip
```c
  char ip[INET_ADDRSTRLEN];
  
  network_get_client_ip(&socket, ip);
```

#### network_server_read_from
```c
  struct sockaddr client_socket = {0};
  char request[BUFFER_SIZE] = {0};

  int val = network_server_read_from(&network, &client_socket, request, BUFFER_SIZE);
```

#### network_server_send_to
```c
  char respond[BUFFER_SIZE] = "respond";
  
  network_server_send_to(&network, &client_socket, respond, BUFFER_SIZE);
```


### Client
#### network_client_init
```c
  switch (network_client_init(&network, UDP, ADDR, PORT)){
    case CONNECTERROR:
      printf("Can't init network\n");
      exit(EXIT_FAILURE);
      break;
    case DNSERROR:
      printf("Can't find ip from dns!\n");
      exit(EXIT_FAILURE);
      break;
    default: 
      printf("Client initalized!\n");
      break;
  };
```

#### network_client_destroy
```c
  network_client_destroy(&network);
```

#### network_client_connect
```c
switch (network_client_connect(&network)){
  case CONNECTERROR:
    printf("Can't connect to network!\n");
    network_client_destroy(&network);
    printf("Network destroyed!\n");
    exit(EXIT_FAILURE);
    break;
  default:
    printf("Connected to server!\n");  
    break;
};
```

#### network_client_read
```c
  char respond[BUFFER_SIZE + NAMESIZE] = {0};

  network_client_read(&network, respond, BUFFER_SIZE + NAMESIZE)
```

#### network_client_send
```c
  char send_request[NAMESIZE + BUFFER_SIZE] = "request";
  
  network_client_send(network, send_request, NAMESIZE + BUFFER_SIZE);
```

#### network_client_read_from
```c
  char respond[NAMESIZE + BUFFER_SIZE] = {0};
  
  int val = network_client_read_from(network_provider, respond, NAMESIZE + BUFFER_SIZE);
    
  for(unsigned int i = 0; i < 100; i++) {
    if(val == NODATA) {
      sleep(1/20);
      val = network_client_read_from(network_provider, respond, NAMESIZE + BUFFER_SIZE);
    };
  };

  printf("%s\n", respond);
```

#### network_client_send_to
```c
  char request[NAMESIZE + BUFFER_SIZE] = "request";
  
  network_client_send_to(network_provider, request, NAMESIZE + BUFFER_SIZE)
```

#### network_client_send_request
```c
  char respond[BUFFER_SIZE] = {0};
  
  network_client_send_request(&network, request, BUFFER_SIZE, respond, 20, 100);

  printf("%s\n", respond);
```

### Server/Client
#### network_check_if_empty_message
```c
  if(network_check_if_empty_message(request) == NODATA){
    printf("Can't send empty message\n");
  };
```



## Small Example
- [TCP Example](Examples/TCP/)
- [UDP Example](Examples/UDP/)


## Full Example
- [Chat Example](Example/Chat/)



## Features
- **Non-blocking I/O**: No need to wait for data to be read or sent; the program can continue executing while awaiting network events.
- **Multiple Client Handling**: The server can handle multiple client connections simultaneously.
- **Protocols**: TCP and UDP protocol
- **Cross-Platform**: Windows and Linux supported.
- **Working Example**: Ran server in cloud. So many users can connect and chat with each other. 



## License
[GNU GENERAL PUBLIC LICENSE Version 2, June 1991](LICENSE)




## Prerequisites
- CMake 3.15 or higher
- Git (for cloning with submodules)