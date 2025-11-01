#ifndef PLATFORM_H
#define PLATFORM_H

#include "../../Macro.h"

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




void client_noblocking_get_input(){
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

int client_noblocking_get_input(char* request){
  if (fgets(request, BUFFER_SIZE, stdin) != NULL) {
    // change new line to null terminator
    request[strcspn(request, "\n")] = 0;

    return 1;
  };

  return 0;
};

#endif
#endif