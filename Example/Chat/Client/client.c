#include "client.h"

#define PORT 9090
#define ADDR "tin-s498831.vm.wmi.amu.edu.pl"
// #define ADDR "127.0.0.1"

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

#ifdef WIN32
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
#endif






int main(){
  signal(SIGINT, sigint_handler);

  if(network_client_init(&network, ADDR, PORT) == ERRORCODE)
    printf("Can't init network\n");

  if(network_client_connect(&network) == ERRORCODE){
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

#ifndef WIN32
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
#endif



  printf("> ");
  while (running) {
#ifdef _WIN32
    if (_kbhit()) {
      int ch = _getch();
      if (ch == '\r' || ch == '\n') {
        send_buffer[strlen(send_buffer)] = '\0';
        if (strcmp(send_buffer, "exit") == 0) {
          running = 0;
          break;
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
#else
    if (fgets(send_buffer, BUFFER_SIZE, stdin) != NULL) {
      send_buffer[strcspn(send_buffer, "\n")] = 0;
      if (strcmp(send_buffer, "exit") == 0) {
        running = 0;
        break;
      };

      char buffer[NAMESIZE + BUFFER_SIZE] = {0};
      strcat(buffer, name);
      strcat(buffer, send_buffer);


      if(send_buffer == NULL || strlen(send_buffer) == 0 || send_buffer[0] == '\0')
        printf("Can't send empty message\n");
      else
        network_client_send(&network, buffer, strlen(buffer));
      
      printf("> ");
      memset(send_buffer, 0, BUFFER_SIZE);
    };
#endif

    int valread = network_client_read(&network, read_buffer, BUFFER_SIZE + NAMESIZE);
    if (valread == DISCONNECT) {
      printf("\nServer closed the connection.\n");
      running = 0;
    } 
    else if (valread > 0) {
      printf("%s\n> ", read_buffer);
      memset(read_buffer, 0, BUFFER_SIZE + NAMESIZE);
    };

#ifdef _WIN32
    Sleep(50);
#else
    usleep(50000);
#endif
  };


  network_client_destroy(&network);
  return 0;
};

