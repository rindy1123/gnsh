#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "gnsh.h"

char **
createArgs(char *line)
{
  char *temp_string = NULL;
  char *separated_string = strdup(line);
  char **command_args = (char **)malloc(BUFF * sizeof(char *));
  for (int i = 0; (temp_string = strsep(&separated_string, " \t")) != NULL; i++)
    command_args[i] = temp_string;
  free(temp_string);
  free(separated_string);
  return command_args;
}

void execCommand(char *path, char **command_args)
{
  pid_t pid = fork();
  if (pid < 0)
  {
    fprintf(stderr, "error occurs");
    exit(1);
  }
  else if (pid == 0)
  {
    if (execv(path, command_args) == -1)
    {
      fprintf(stderr, "error occurs");
      exit(1);
    }
  }
  else
  {
    waitpid(pid, NULL, 0);
  }
}

int main(int argc, char *argv[])
{
  FILE *in;
  FILE *out;
  if (argc == 1)
  {
    in = stdin;
    out = stdout;
    char *line;
    size_t linecap = 0;
    while (1)
    {
      fprintf(out, "gnsh> ");
      getline(&line, &linecap, in);
      // remove new line char
      line[strcspn(line, "\n")] = 0;
      if (strcmp(line, "\0") == 0)
        continue;

      char *command;
      char **command_args = createArgs(line);
      command = strdup(command_args[0]);

      char *bin_path = (char *)malloc(strlen(command) + strlen(BIN_PATH) + 1);
      sprintf(bin_path, "%s/%s", BIN_PATH, command);
      if (access(bin_path, X_OK) == 0)
      {
        execCommand(bin_path, command_args);
        free(bin_path);
        continue;
      }
      char *usr_bin_path = (char *)malloc(strlen(command) + strlen(USR_BIN_PATH) + 1);
      sprintf(usr_bin_path, "%s/%s", USR_BIN_PATH, command);
      if (access(usr_bin_path, X_OK) == 0)
      {
        fprintf(out, "%s\n", usr_bin_path);
        free(bin_path);
        free(usr_bin_path);
        continue;
      }
      if (strcmp(command, "exit") == 0)
      {
        free(usr_bin_path);
        free(bin_path);
        return 0;
      }
      fprintf(stderr, "gnsh: Unknown command: %s\n", command);
      free(usr_bin_path);
      free(bin_path);
    }
  }

  return 0;
}
