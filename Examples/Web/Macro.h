#ifndef MACRO_H
#define MACRO_H

#define PORT 9090
// #define ADDR "api.chat.daynlight.pl"
#define ADDR "127.0.0.1"

#define BUFFER_SIZE 1000
#define NAMESIZE 25



#define print_and_log(fmt, ...) do { \
    printf(fmt, ##__VA_ARGS__); \
    fflush(stdout); \
    FILE *log = fopen("server.log", "a"); \
    if (log) { \
        fprintf(log, fmt, ##__VA_ARGS__); \
        fclose(log); \
    } \
} while(0)

#endif