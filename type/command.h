#include <pthread.h>
typedef struct command {
  pthread_t thread;
  char *command;
} Command;
