#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "utils/utils.h"

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

// Parses a string into args[], respecting quotes and backslashes
// Returns arg_count
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
      case ' ':
        buffer[j++] = ' ';
        break;
      case '\\':
        buffer[j++] = '\\';
        break;
      case '\'':
        buffer[j++] = '\'';
        break;
      case '"':
        buffer[j++] = '"';
        break;
      case 'n':
        buffer[j++] = 'n';
        break;
      default:
        buffer[j++] = input[i];
        break;
      }
      i++;
    }
    else if (input[i] == ' ' && !in_single_quote && !in_double_quote)
    {
      // Space outside quotes = argument boundary
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

  // Grab the last token if any
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
    // Overwrite to stdout
    if (strcmp(args[i], "1>") == 0 || strcmp(args[i], ">") == 0)
    {
      if (args[i + 1] == NULL)
      {
        fprintf(stderr, "Syntax error: expected filename after '>'\n");
        return -1;
      }

      outputFile = args[i + 1];

      for (int j = 0; j < i; j++)
      {
        exeCommand[j] = args[j];
      }

      exeCommand[i] = NULL;

      int saved_stdout = dup(STDOUT_FILENO);
      int fd = open(outputFile, O_CREAT | O_WRONLY | O_TRUNC, 0644);

      dup2(fd, STDOUT_FILENO);
      close(fd);

      execute(exeCommand);

      dup2(saved_stdout, STDOUT_FILENO);
      close(saved_stdout);

      return 1;
    }
    
    // Overwrite to stderr
    if (strcmp(args[i], "2>") == 0)
    {
      if (args[i + 1] == NULL)
      {
        fprintf(stderr, "Syntax error: expected filename after '>'\n");
        return -1;
      }

      outputFile = args[i + 1];

      for (int j = 0; j < i; j++)
      {
        exeCommand[j] = args[j];
      }

      exeCommand[i] = NULL;

      int saved_stderr = dup(STDERR_FILENO);
      int fd = open(outputFile, O_CREAT | O_WRONLY | O_TRUNC, 0644);

      dup2(fd, STDERR_FILENO);
      close(fd);

      execute(exeCommand);

      dup2(saved_stderr, STDERR_FILENO);
      close(saved_stderr);

      return 1;
    }

    // Append to stdout
    if (strcmp(args[i], "1>>") == 0 || strcmp(args[i], ">>") == 0)
    {
      if (args[i + 1] == NULL)
      {
        fprintf(stderr, "Syntax error: expected filename after '>'\n");
        return -1;
      }

      outputFile = args[i + 1];

      for (int j = 0; j < i; j++)
      {
        exeCommand[j] = args[j];
      }

      exeCommand[i] = NULL;

      int saved_stdout = dup(STDOUT_FILENO);
      int fd = open(outputFile, O_CREAT | O_WRONLY | O_APPEND, 0644);

      dup2(fd, STDOUT_FILENO);
      close(fd);

      execute(exeCommand);

      dup2(saved_stdout, STDOUT_FILENO);
      close(saved_stdout);

      return 1;
    }

    // Append to stderr
    if (strcmp(args[i], "2>>") == 0)
    {
      if (args[i + 1] == NULL)
      {
        fprintf(stderr, "Syntax error: expected filename after '>'\n");
        return -1;
      }

      outputFile = args[i + 1];

      for (int j = 0; j < i; j++)
      {
        exeCommand[j] = args[j];
      }

      exeCommand[i] = NULL;

      int saved_stderr = dup(STDERR_FILENO);
      int fd = open(outputFile, O_CREAT | O_WRONLY | O_APPEND, 0644);

      dup2(fd, STDERR_FILENO);
      close(fd);

      execute(exeCommand);

      dup2(saved_stderr, STDERR_FILENO);
      close(saved_stderr);

      return 1;
    }

    i++;
  }

  return 0;
}

int main(int argc, char **argv)
{
  setbuf(stdout, NULL);

  puts("### Shell Version 1.0.0 ###");
  puts("Welcome to my shell! Type 'exit' to quit.\n");

  while (1)
  {
    fputs("> ", stdout);

    char input[MAX_INPUT_SIZE];
    fgets(input, sizeof(input), stdin);
    trimTrailing(input);

    if (strcmp(input, "exit") == 0)
      break;

    char *args[MAX_ARGS];
    int arg_count = parseArgs(input, args);

    int redirect = handleRedirection(args);

    if (redirect == 1)
      continue;

    if (arg_count == 0)
      continue;

    // Builtins
    if (strcmp(args[0], "echo") == 0)
    {
      for (int i = 1; i < arg_count; i++)
      {
        if (i > 1)
          printf(" ");
        printf("%s", args[i]);
      }
      printf("\n");
    }
    else if (strcmp(args[0], "type") == 0)
    {
      if (arg_count < 2)
      {
        fprintf(stderr, "type: missing argument\n");
      }
      else if (!strcmp(args[1], "type") || !strcmp(args[1], "echo") || !strcmp(args[1], "exit") || !strcmp(args[1], "cd"))
      {
        printf("%s is a shell builtin\n", args[1]);
      }
      else
      {
        char *filePath = locateExecFile(args[1]);
        if (strlen(filePath) > 0)
          printf("%s is %s\n", args[1], filePath);
        else
          printf("%s: not found\n", args[1]);
      }
    }
    else if (strcmp(args[0], "cd") == 0)
    {
      char *dir = arg_count >= 2 ? args[1] : getenv("HOME");

      if (strcmp(dir, "~") == 0)
        dir = getenv("HOME");

      if (chdir(dir) != 0)
        fprintf(stderr, "%s: No such file or directory\n", dir);
    }
    else
    {
      execute(args);
    }

    // Free strdup'd args
    for (int i = 0; i < arg_count; i++)
      free(args[i]);
  }

  return 0;
}