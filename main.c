#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/utils.h"
#include "builtins/builtins.h"

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

int main(int argc, char **argv)
{
  // Disable buffering for stdout to ensure prompt appears immediately
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

    if (arg_count == 0)
      continue;

    // Handle redirection before builtins/execution
    int redirect = handleRedirection(args);

    if (redirect != 1)
    {
      if (strcmp(args[0], "echo") == 0)
        builtinEcho(args, arg_count);
      else if (strcmp(args[0], "type") == 0)
        builtinType(args, arg_count);
      else if (strcmp(args[0], "cd") == 0)
        builtinCd(args, arg_count);
      else
        execute(args);
    }

    // always runs, no early exit
    for (int i = 0; i < arg_count; i++)
      free(args[i]);
  }

  return 0;
}