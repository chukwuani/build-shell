#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "utils.h"

void trimTrailing(char *str)
{
  int len = strlen(str);
  while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || str[len - 1] == ' '))
  {
    str[--len] = '\0';
  }
}

char *locateExecFile(const char *name)
{
  static char fullPath[MAX_INPUT_SIZE];
  char *pathEnv = getenv("PATH");

  if (!pathEnv)
    return "";

  char *pathCopy = strdup(pathEnv);
  char *dir = strtok(pathCopy, ":");

  while (dir != NULL)
  {
    snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, name);
    if (access(fullPath, X_OK) == 0)
    {
      free(pathCopy);
      return fullPath;
    }
    dir = strtok(NULL, ":");
  }

  free(pathCopy);
  return "";
}

void execute(char **args)
{
  pid_t pid = fork();

  if (pid == 0)
  {
    execvp(args[0], args);
    fprintf(stderr, "%s: command not found\n", args[0]);
    exit(EXIT_FAILURE);
  }
  else if (pid > 0)
  {
    wait(NULL);
  }
  else
  {
    fprintf(stderr, "Error: fork failed\n");
  }
}

int parseArgs(char *input, char **args)
{
  int arg_count = 0;
  int i = 0;
  int in_single_quote = 0;
  int in_double_quote = 0;
  char buffer[MAX_INPUT_SIZE];
  int j = 0;

  while (input[i] != '\0' && arg_count < MAX_ARGS - 1)
  {
    if (input[i] == '\'' && !in_double_quote)
    {
      in_single_quote = !in_single_quote;
      i++;
    }
    else if (input[i] == '"' && !in_single_quote)
    {
      in_double_quote = !in_double_quote;
      i++;
    }
    else if (input[i] == '\\' && !in_single_quote)
    {
      i++;
      switch (input[i])
      {
        case ' ':  buffer[j++] = ' ';  break;
        case '\\': buffer[j++] = '\\'; break;
        case '\'': buffer[j++] = '\''; break;
        case '"':  buffer[j++] = '"';  break;
        case 'n':  buffer[j++] = 'n';  break;
        default:   buffer[j++] = input[i]; break;
      }
      i++;
    }
    else if (input[i] == ' ' && !in_single_quote && !in_double_quote)
    {
      if (j > 0)
      {
        buffer[j] = '\0';
        args[arg_count++] = strdup(buffer);
        j = 0;
      }
      i++;
    }
    else
    {
      buffer[j++] = input[i++];
    }
  }

  if (j > 0)
  {
    buffer[j] = '\0';
    args[arg_count++] = strdup(buffer);
  }

  args[arg_count] = NULL;
  return arg_count;
}

int handleRedirection(char **args)
{
  int i = 0;
  char *outputFile;
  char *exeCommand[MAX_ARGS];

  while (args[i] != NULL)
  {
    int is_stdout_overwrite = strcmp(args[i], ">")   == 0 || strcmp(args[i], "1>")  == 0;
    int is_stderr_overwrite = strcmp(args[i], "2>")  == 0;
    int is_stdout_append    = strcmp(args[i], ">>")  == 0 || strcmp(args[i], "1>>") == 0;
    int is_stderr_append    = strcmp(args[i], "2>>") == 0;

    if (is_stdout_overwrite || is_stderr_overwrite || is_stdout_append || is_stderr_append)
    {
      if (args[i + 1] == NULL)
      {
        fprintf(stderr, "Syntax error: expected filename after redirection\n");
        return -1;
      }

      outputFile = args[i + 1];

      for (int j = 0; j < i; j++)
        exeCommand[j] = args[j];
      exeCommand[i] = NULL;

      int flags = O_CREAT | O_WRONLY | (is_stdout_append || is_stderr_append ? O_APPEND : O_TRUNC);
      int fd = open(outputFile, flags, 0644);

      if (fd == -1)
      {
        fprintf(stderr, "Error: cannot open file '%s'\n", outputFile);
        return -1;
      }

      int target_fd = (is_stderr_overwrite || is_stderr_append) ? STDERR_FILENO : STDOUT_FILENO;
      int saved_fd  = dup(target_fd);

      dup2(fd, target_fd);
      close(fd);

      execute(exeCommand);

      dup2(saved_fd, target_fd);
      close(saved_fd);

      return 1;
    }

    i++;
  }

  return 0;
}