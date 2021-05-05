#include "type/pathlist.h"
#define BUFF 50
#define BIN_PATH "/bin"
#define USR_BIN_PATH "/usr/bin"

char **createArgs(char *line, int *args_num);
void execCommand(char *path, char **command_args);
void warnUnknownCommand(char *command);
int handleDefaultCommand(char *command, char **command_args, PathList *path_list_head);
int handleBuiltIn(char *command, char **command_args, PathList **path_list_head);
