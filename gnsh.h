#define BUFF 50
#define BIN_PATH "/bin"
#define USR_BIN_PATH "/usr/bin"

char **createArgs(char *line);
void execCommand(char *path, char **command_args);
void warnUnknownCommand(char *command);
typedef struct pathList {
  char *path;
  struct pathList *next;
} PathList;
int handleInput(char *command, char **command_args, PathList *path_list_head);
PathList *initializePathList(void);
