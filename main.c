#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils/utils.h"

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

int main(int argc, char **argv)
{
  // Flush after every printf
  setbuf(stdout, NULL);

  puts("### Shell Version 1.0.0 ###");
  puts("Welcome to my shell! Type 'exit' to quit.\n");

  // Infinite shell loop
  while (1)
  {
    fputs("> ", stdout);

    // Read a line of input from the user
    char input[MAX_INPUT_SIZE];
    fgets(input, sizeof(input), stdin);

    // Remove the newline character if present
    trimTrailing(input);

    // Start checking builtins
    if (strcmp(input, "exit") == 0)
    {
      break;
    }
    else if (strncmp(input, "echo ", 5) == 0)
    {
      char arg[MAX_INPUT_SIZE];
      strcpy(arg, input + 5);

      if (arg[0] == '\'')
      {
        // Check if the argument is enclosed in single quotes
        if (arg[0] == '\'' && arg[strlen(arg) - 1] == '\'')
        {
          // Remove the single quotes from the back
          // Copy the remain content except the first quote back into arg
          arg[strlen(arg) - 1] = '\0';
          memmove(arg, arg + 1, strlen(arg));
        }
        else
        {
          // Else split by delimeter ' ' and reconstruct the string with spaces in between
          char *temp = strdup(arg);
          char result[MAX_INPUT_SIZE] = "";

          char *token = strtok(temp, " ");

          while (token != NULL)
          {
            strcat(result, token);
            strcat(result, " ");
            token = strtok(NULL, " ");
          }

          strcpy(arg, result);
          free(temp);
        }

        // Split by delimeter '\'' and reconstruct the string
        char *temp = strdup(arg);
        char result[MAX_INPUT_SIZE] = "";

        char *token = strtok(temp, "\'");

        while (token != NULL)
        {
          trimTrailing(token);
          strcat(result, token);
          token = strtok(NULL, "\'");
        }

        strcpy(arg, result);
        free(temp);

        // Remove trailing spaces from the argument
        trimTrailing(arg);
      }
      else if (arg[0] == '"')
      {
        // Check if the argument is enclosed in double quotes
        if (arg[0] == '"' && arg[strlen(arg) - 1] == '"')
        {
          // Remove the single quotes from the back
          // Copy the remain content except the first quote back into arg
          arg[strlen(arg) - 1] = '\0';
          memmove(arg, arg + 1, strlen(arg));
        }
        else
        {
          char *temp = strdup(arg);
          char result[MAX_INPUT_SIZE] = "";

          char *token = strtok(temp, " ");

          while (token != NULL)
          {
            strcat(result, token);
            strcat(result, " ");
            token = strtok(NULL, " ");
          }

          strcpy(arg, result);
          free(temp);
        }

        // Split by delimeter '"' and reconstruct the string
        char *temp = strdup(arg);
        char result[MAX_INPUT_SIZE] = "";

        char *token = strtok(temp, "\"");

        while (token != NULL)
        {
          strcat(result, token);
          token = strtok(NULL, "\"");
        }

        strcpy(arg, result);
        free(temp);

        // Remove trailing spaces from the argument
        trimTrailing(arg);
      }

      // Print the argument after "echo "
      printf("%s\n", arg);
    }
    else if (strncmp(input, "type ", 5) == 0)
    {
      char arg[MAX_INPUT_SIZE];
      strcpy(arg, input + 5);

      // For builtin inputs like echo...
      if (!strcmp(arg, "type") || !strcmp(arg, "echo") || !strcmp(arg, "exit"))
      {
        printf("%s is a shell builtin\n", arg);
      }
      else
      {
        // Check if argument is an exec file
        char *filePath = locateExecFile(arg);

        if (strlen(filePath) > 0)
        {
          printf("%s is %s\n", arg, filePath);
        }
        else
        {
          printf("%s: not found\n", arg);
        }
      }
    }
    else if (strncmp(input, "cd ", 3) == 0)
    {
      char dir[MAX_INPUT_SIZE];
      strcpy(dir, input + 3);

      // Check for "~" and replace with home directory
      if (strcmp(dir, "~") == 0)
      {
        char *homeDir = getenv("HOME");

        if (homeDir != NULL)
          strcpy(dir, homeDir);
      }

      if (chdir(dir) != 0)
        fprintf(stderr, "%s: No such file or directory\n", dir);
    }
    else
    {
      char *token = strtok(input, " \t");
      char *args[MAX_ARGS]; // This is our own "argv"
      int arg_count = 0;    // This is our own "argc"

      while (token != NULL && arg_count < MAX_ARGS - 1)
      {
        args[arg_count++] = token;
        token = strtok(NULL, " \t");
      }

      args[arg_count] = NULL;

      if (arg_count == 0)
        continue;

      // Execute
      execute(args);
    }
  }

  return 0;
}
