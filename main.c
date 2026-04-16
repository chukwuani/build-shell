#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "utils/utils.h"
#include "builtins/builtins.h"

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

#define ECHO_CMD "echo"
#define EXIT_CMD "exit"
#define QUIT_CMD "quit"
#define TYPE_CMD "type"
#define PWD_CMD "pwd"
#define CHDIR_CMD "cd"

char *builtins[] = {
    ECHO_CMD,
    EXIT_CMD,
    QUIT_CMD,
    TYPE_CMD,
    PWD_CMD,
    CHDIR_CMD};

char *custom_generator(const char *text, int state)
{
  static int list_index, len;
  char *match;

  if (!state)
  {
    list_index = 0;
    len = strlen(text);
  }

  while (builtins[list_index] != NULL)
  {
    match = builtins[list_index++];

    if (strncmp(match, text, len) == 0)
    {
      return strdup(match);
    }
  }

  return NULL;
}

char **custom_completion(const char *text, int start, int end)
{
  rl_attempted_completion_over = 1; // Prevent filename completion fallback
  return rl_completion_matches(text, custom_generator);
}

int main(int argc, char **argv)
{
  // Flush after every printf
  setbuf(stdout, NULL);

  puts("### Shell Version 1.0.0 ###");
  puts("Welcome to my shell! Type 'exit' to quit.\n");

  rl_attempted_completion_function = custom_completion;

  while (1)
  {
    // readline handles prompt + input, returns heap allocated string
    char *input = readline("> ");

    if (input == NULL)
      break;

    if (strlen(input) == 0)
    {
      free(input);
      continue;
    }

    // Add to history so up arrow works
    add_history(input);

    if (strcmp(input, "exit") == 0)
    {
      free(input);
      break;
    }

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

    // Free readline's heap allocation
    free(input);
  }

  return 0;
}