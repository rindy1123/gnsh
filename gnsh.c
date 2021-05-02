#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#define BIN_PATH "/bin"
#define USR_BIN_PATH "/usr/bin"

int
main(int argc, char *argv[])
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
      command = line;

      char *bin_path = (char *)malloc(strlen(command) + strlen(BIN_PATH) + 1);
      sprintf(bin_path, "%s/%s", BIN_PATH, command);
      char **command_args = (char **)malloc((strlen(bin_path) + 30) * sizeof(char *));
      // char *command_args[3] = {bin_path, "-a", NULL};
      command_args[0] = bin_path;
      command_args[1] = "-a";
      command_args[2] = NULL;
      if (access(bin_path, X_OK) == 0)
      {
        pid_t pid = fork();
        if (pid < 0)
        {
          fprintf(stderr, "error occurs");
          exit(1);
        }
        else if (pid == 0)
        {
          if (execv(bin_path, command_args) == -1)
          {
            fprintf(stderr, "error occurs");
            exit(1);
          }
        }
        else
        {
          waitpid(pid, NULL, 0);
          fprintf(out, "%s\n", bin_path);
          free(bin_path);
          continue;
        }
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
