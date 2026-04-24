#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <dirent.h>

#include "utils/utils.h"
#include "builtins/builtins.h"

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64
int start_index = 0;

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
char **executables;

void load_path_commands()
{
  char *path = getenv("PATH");
  char *pathCopy;

  if (!path)
    return;

  pathCopy = strdup(path);
  char *token = strtok(pathCopy, ":");

  while (token != NULL)
  {
    DIR *dir = opendir(token);

    if (!dir)
    {
      token = strtok(NULL, ":");
      continue;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)))
    {
      if (entry->d_name[0] == '.')
        continue;

      executables = realloc(executables, (start_index + 1) * sizeof(char *));
      executables[start_index++] = strdup(entry->d_name);
    }

    closedir(dir);

    token = strtok(NULL, ":");
  }

  free(pathCopy);
}

char *custom_generator(const char *text, int state)
{
  static int list_index, exec_index, len, found;
  char *match;

  if (!state)
  {
    list_index = 0;
    exec_index = 0;
    len = strlen(text);
    found = 0;
  }

   if (found) return NULL;

  while (builtins[list_index] != NULL)
  {
    match = builtins[list_index++];

    if (strncmp(match, text, len) == 0)
    {
      found = 1;
      return strdup(match);
    }
  }

  while (exec_index < start_index)
  {
    match = executables[exec_index++];

    if (strncmp(match, text, len) == 0)
    {
      found = 1;
      return strdup(match);
    }
  }

  return NULL;
}

char **custom_completion(const char *text, int start, int end)
{
  rl_attempted_completion_over = 1;

  char **matches = rl_completion_matches(text, custom_generator);

  if (matches == NULL)
    rl_ding(); // bell on no matches

  return matches;
}

int main(int argc, char **argv)
{
  // Flush after every printf
  setbuf(stdout, NULL);

  puts("### Shell Version 1.0.0 ###");
  puts("Welcome to my shell! Type 'exit' to quit.\n");

  load_path_commands();

  printf("Loaded %d executables from PATH\n", start_index);

  rl_completion_display_matches_hook = NULL;
  rl_bind_key('\t', rl_complete);
  rl_variable_bind("bell-style", "audible");
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