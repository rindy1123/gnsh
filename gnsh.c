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

      char *bin_path = (char *)malloc(strlen(line) + strlen(BIN_PATH) + 1);
      sprintf(bin_path, "%s/%s", BIN_PATH, line);
      if (access(bin_path, X_OK) == 0)
        fprintf(out, "%s\n", bin_path);
      else
      {
        char *usr_bin_path = (char *)malloc(strlen(line) + strlen(USR_BIN_PATH) + 1);
        sprintf(usr_bin_path, "%s/%s", USR_BIN_PATH, line);
        if (access(usr_bin_path, X_OK) == 0)
          fprintf(out, "%s\n", usr_bin_path);
        else
        {
          if (strcmp(line, "exit") == 0)
          {
            free(usr_bin_path);
            return 0;
          }
          fprintf(stderr, "gnsh: Unknown command: %s\n", line);
        }
        free(usr_bin_path);
      }
      free(bin_path);
    }
  }

  return 0;
}
