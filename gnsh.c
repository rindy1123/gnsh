#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "gnsh.h"

char **
createArgs(char *line, int *args_num)
{
  char *temp_string = NULL;
  char *separated_string = strdup(line);
  char **command_args = (char **)malloc(BUFF * sizeof(char *));
  int i;
  for (i = 0; (temp_string = strsep(&separated_string, " \t")) != NULL; i++)
    command_args[i] = temp_string;
  *args_num = i;
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

void warnUnknownCommand(char *command)
{
  fprintf(stderr, "gnsh: Unknown command: %s\n", command);
}

/*
  return value == 0: succeeded
  return value == -1: failed
*/
int handleDefaultCommand(char *command, char **command_args, PathList *path_list_head)
{
  PathList *path_list = path_list_head;
  while (path_list != NULL)
  {
    char *bin_path = (char *)malloc(strlen(command) + strlen(path_list->path) + 1);
    sprintf(bin_path, "%s/%s", path_list->path, command);
    if (access(bin_path, X_OK) == 0)
    {
      execCommand(bin_path, command_args);
      free(bin_path);
      return 0;
    }
    free(bin_path);
    path_list = path_list->next;
  }
  return -1;
}

/*
  return value == 0: built-in command exists
  return value == -1: built-in command does not exist
*/
int handleBuiltIn(char *command, char **command_args, PathList **path_list_head)
{
  if (strcmp(command, "exit") == 0)
    exit(EXIT_SUCCESS);
  else if (strcmp(command, "cd") == 0)
  {
    if (command_args[2] != NULL)
      fprintf(stderr, "Too many args for cd command\n");
    else if (command_args[1] == NULL)
      fprintf(stderr, "Specify a path to the directory\n");
    else if (chdir(command_args[1]) == -1)
      fprintf(stderr, "cd: The directory '%s' does not exist\n", command_args[1]);
    return 0;
  }
  else if (strcmp(command, "path") == 0)
  {
    if (command_args[1] == NULL)
    {
      // reset paths
      PathList *temp_path_list = malloc(sizeof(PathList));
      while (*path_list_head != NULL)
      {
        temp_path_list = *path_list_head;
        *path_list_head = temp_path_list->next;
      }
      free(temp_path_list);
    }
    else
    {
      // add paths
      for (int i = 1; command_args[i] != NULL; i++)
      {
        PathList *next_path_list = malloc(sizeof(PathList));
        if (next_path_list == NULL)
        {
          fprintf(stderr, "malloc failed\n");
          exit(EXIT_FAILURE);
        }
        next_path_list->path = strdup(command_args[i]);
        next_path_list->next = *path_list_head;
        *path_list_head = next_path_list;
      }
    }
    return 0;
  }
  return -1;
}

PathList *initializePathList(void)
{
  PathList *path_list = malloc(sizeof(PathList));
  path_list->path = BIN_PATH;
  path_list->next = NULL;
  return path_list;
}

int main(int argc, char *argv[])
{
  FILE *in;
  FILE *out;
  char *line;
  size_t linecap = 0;
  PathList *path_list_head = initializePathList();
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

      int *args_num = (int *)malloc(sizeof(int *));
      char **command_args = createArgs(line, args_num);
      char *command = strdup(command_args[0]);
      if ((handleBuiltIn(command, command_args, &path_list_head)) == 0)
        continue;
      if ((handleDefaultCommand(command, command_args, path_list_head)) == 0)
        continue;
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

      int *args_num = (int *)malloc(sizeof(int *));
      char **command_args = createArgs(line, args_num);
      char *command = strdup(command_args[0]);

      if ((handleBuiltIn(command, command_args, &path_list_head)) == 0)
        continue;
      if ((handleDefaultCommand(command, command_args, path_list_head)) == 0)
        continue;
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
