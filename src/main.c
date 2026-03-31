#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);

  while (1)
  {
    printf("$ ");

    // Read a line of input from the user
    char command[1024];
    fgets(command, sizeof(command), stdin);

    // Remove the newline character if present
    command[strcspn(command, "\n")] = '\0';

    // Check if the command is "exit"
    if (strcmp(command, "exit") == 0)
    {
      break;
    }
    else if (strncmp(command, "echo ", 5) == 0)
    {
      // Print the argument after "echo "
      printf("%s\n", command + 5);
    }
    else if (strncmp(command, "type ", 5) == 0)
    {
      char arg[1024];
      strcpy(arg, command + 5);

      if (strcmp(arg, "type") == 0 || strcmp(arg, "echo") == 0 || strcmp(arg, "exit") == 0)
      {
        printf("%s is a shell builtin\n", arg);
      }
      else
      {
        printf("%s: not found\n", arg);
      }
    }
    else
    {
      // For simplicity, we will just print the command back to the user
      printf("%s: command not found\n", command);
    }
  }

  return 0;
}
