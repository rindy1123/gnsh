#include <pthread.h>
struct command {
  pthread_t thread;
  char *command;
};
