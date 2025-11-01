#ifndef GETIO_H
#define GETIO_H

#include <signal.h>


void client_commands(char* request){
  if (strcmp(request, "exit") == 0) {   // exit command
    running = 0;
    return;
  };
};


void send_request(char *message){
  // change new line to null terminator
  message[strcspn(message, "\n")] = 0;
  
  // run client commands
  client_commands(message);
  
  // send message buffer
  char request[NAMESIZE + BUFFER_SIZE] = {0};
  
  if (message[0] == '@'){         // Private Message    
    request[0] = '3';
    strcat(request + 1, message);
  }
  else{                               // Public Message
    request[0] = '2';
    strcat(request + 1, message);
  }
    
  // check if message is valid
  if(network_check_if_empty_message(message) == NODATA){
    printf("Can't send empty message\n");
    return;
  };

  // compress request
  char compressed_request[NAMESIZE + BUFFER_SIZE];
  compression_rle_compress(request, compressed_request);

  // send request
  network_client_send(&network, compressed_request, strlen(compressed_request));
  
  printf("> ");
};


#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include <conio.h>


void set_stdin_nonblocking(void) {
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode;

  // Get current console mode
  if (!GetConsoleMode(hStdin, &mode)) {
    fprintf(stderr, "GetConsoleMode() failed\n");
    return;
  }

  // Disable line buffering and echo if desired
  mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);

  if (!SetConsoleMode(hStdin, mode)) {
    fprintf(stderr, "SetConsoleMode() failed\n");
  }
}




void client_get_input(){
  if (_kbhit()) {
    int ch = _getch();
    if (ch == '\r' || ch == '\n') {
      send_buffer[strlen(send_buffer)] = '\0';
      if (strcmp(send_buffer, "exit") == 0) {
        running = 0;
        return;
      };

      char buffer[NAMESIZE + BUFFER_SIZE] = {0};
      strcat(buffer, name);
      strcat(buffer, send_buffer);

      if(send_buffer == NULL || strlen(send_buffer) == 0 || send_buffer[0] == '\0')
        printf("Can't send empty message\n");
      else
        network_client_send(&network, buffer, strlen(buffer));

      printf("\n> ");
      memset(send_buffer, 0, BUFFER_SIZE);
    } 
    else if (ch == 8) { // backspace
      size_t len = strlen(send_buffer);
      if (len > 0) {
        send_buffer[len - 1] = '\0';
        printf("\b \b");
        fflush(stdout);
      };
    } 
    else {
      size_t len = strlen(send_buffer);
      if (len < BUFFER_SIZE - 1)
        send_buffer[len] = (char)ch, send_buffer[len + 1] = '\0';
      printf("%c", ch);
    };
  };
};

#else

int client_noblocking_get_input(char* buffer){
  if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
    return 1;
  };
};

#endif


#endif