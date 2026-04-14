#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "../utils/utils.h"

void builtinEcho(char **args, int arg_count)
{
  for (int i = 1; i < arg_count; i++)
  {
    if (i > 1) printf(" ");
    printf("%s", args[i]);
  }
  printf("\n");
}

void builtinType(char **args, int arg_count)
{
  if (arg_count < 2)
  {
    fprintf(stderr, "type: missing argument\n");
    return;
  }

  const char *builtins[] = { "echo", "type", "exit", "cd", NULL };

  for (int i = 0; builtins[i] != NULL; i++)
  {
    if (strcmp(args[1], builtins[i]) == 0)
    {
      printf("%s is a shell builtin\n", args[1]);
      return;
    }
  }

  char *filePath = locateExecFile(args[1]);
  if (strlen(filePath) > 0)
    printf("%s is %s\n", args[1], filePath);
  else
    printf("%s: not found\n", args[1]);
}

void builtinCd(char **args, int arg_count)
{
  char *dir = arg_count >= 2 ? args[1] : getenv("HOME");

  if (dir == NULL)
  {
    fprintf(stderr, "cd: HOME not set\n");
    return;
  }

  if (strcmp(dir, "~") == 0)
    dir = getenv("HOME");

  if (chdir(dir) != 0)
    fprintf(stderr, "%s: No such file or directory\n", dir);
}