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
    fprintf(stderr, "error occured\n");
    exit(1);
  }
  else if (pid == 0)
  {
    if (execv(path, command_args) == -1)
    {
      fprintf(stderr, "error occured\n");
      exit(1);
    }
  }
  else
  {
    waitpid(pid, NULL, 0);
  }
}

/*
  return value == 0: succeeded
  return value == -1: failed
*/
int handleInput(char *command, char **command_args)
{
  char *bin_path = (char *)malloc(strlen(command) + strlen(BIN_PATH) + 1);
  sprintf(bin_path, "%s/%s", BIN_PATH, command);
  if (access(bin_path, X_OK) == 0)
  {
    // /bin
    execCommand(bin_path, command_args);
    free(bin_path);
    return 0;
  }
  free(bin_path);
  char *usr_bin_path = (char *)malloc(strlen(command) + strlen(USR_BIN_PATH) + 1);
  sprintf(usr_bin_path, "%s/%s", USR_BIN_PATH, command);
  if (access(usr_bin_path, X_OK) == 0)
  {
    // /usr/bin
    execCommand(usr_bin_path, command_args);
    free(usr_bin_path);
    return 0;
  }
  free(usr_bin_path);
  return -1;
}

void warnUnknownCommand(char *command)
{
  fprintf(stderr, "gnsh: Unknown command: %s\n", command);
}

int main(int argc, char *argv[])
{
  FILE *in;
  FILE *out;
  char *line;
  size_t linecap = 0;
  PathList *path_list = malloc(sizeof(PathList));
  path_list->path = BIN_PATH;
  path_list->next = NULL;
  if (argc == 1)
  {
    // interactive mode
    in = stdin;
    out = stdout;
    while (1)
    {
      fprintf(out, "gnsh> ");
      getline(&line, &linecap, in);
      // remove new line char
      line[strcspn(line, "\n")] = 0;
      if (strcmp(line, "\0") == 0)
        continue;

      char **command_args = createArgs(line);
      char *command = strdup(command_args[0]);
      // built-in commands
      if (strcmp(command, "exit") == 0)
        return 0;
      else if (strcmp(command, "cd") == 0)
      {
        if (command_args[2] != NULL)
        {
          fprintf(stderr, "Too many args for cd command\n");
          continue;
        }
        else if (command_args[1] == NULL)
        {
          fprintf(stderr, "Specify a path to the directory\n");
          continue;
        }
        if (chdir(command_args[1]) == -1)
        {
          fprintf(stderr, "cd: The directory '%s' does not exist\n", command_args[1]);
          continue;
        }
        continue;
      }
      else if (strcmp(command, "path") == 0)
      {
        printf("%s\n", path_list->path);
        continue;
      }

      int result = handleInput(command, command_args);
      if (result == 0)
        continue;
      else
        warnUnknownCommand(command);
    }
  }
  else if (argc == 2)
  {
    // batch mode
    in = fopen(argv[1], "r");
    while (getline(&line, &linecap, in) != -1)
    {
      // remove new line char
      line[strcspn(line, "\n")] = 0;
      if (strcmp(line, "\0") == 0)
        continue;
      char **command_args = createArgs(line);
      char *command = strdup(command_args[0]);

      int result = handleInput(command, command_args);
      if (result == 0)
        continue;
      else
        warnUnknownCommand(command);
    }
  }
  else
  {
    fprintf(stderr, "Only one input file is supported");
    exit(1);
  }

  return 0;
}
