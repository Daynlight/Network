#ifndef PLATFORM_H
#define PLATFORM_H

#include "../Macro.h"

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




int client_noblocking_get_input(char* request){
  if (_kbhit()) {
    int ch = _getch();
    if (ch == '\r' || ch == '\n') {
      request[strcspn(request, "\n")] = '\0';

      printf("\n> ");
      return 1;
    } 
    else if (ch == 8) { // backspace
      size_t len = strcspn(request, "\n");
      if (len > 0) {
        request[len - 1] = '\0';
        printf("\b \b");
        fflush(stdout);
        return 0;
      };
    } 
    else {
      size_t len = strcspn(request, "\n");
      if (len < BUFFER_SIZE - 1)
        request[len] = (char)ch, request[len + 1] = '\0';
      printf("%c", ch);
      return 0;
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