typedef struct pathList
{
  char *path;
  struct pathList *next;
} PathList;
PathList *initializePathList(void);
