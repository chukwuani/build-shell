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
    else
    {
      char *token = strtok(input, " \t");
      char *args[MAX_ARGS]; // This is our own "argv"
      int arg_count = 0;  // This is our own "argc"

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

      // For simplicity, we will just print the input back to the user
      // printf("%s: input not found\n", input);
    }
  }

  return 0;
}
