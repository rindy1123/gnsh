#define BUFF 50
#define BIN_PATH "/bin"
#define USR_BIN_PATH "/usr/bin"

char **createArgs(char *line);
void execCommand(char *path, char **command_args);
