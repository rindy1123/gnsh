#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define BASE_PATH "/bin"

int
main(int argc, char *argv[])
{
  FILE *in;
  FILE *out;
  char *path = BASE_PATH;
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
      if (strcmp(line, "exit\n") == 0)
        return 0;
      fprintf(out, "%s/%s", path, line);
    }
  }

  return 0;
}
