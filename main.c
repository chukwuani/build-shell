#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  // Flush after every printf
  setbuf(stdout, NULL);

  puts("### Shell Version 1.0.0 ###");
  puts("Welcome to my shell! Type 'exit' to quit.\n");

  // Infinite loop to read commands from the user
  while (1)
  {
    fputs("> ", stdout);

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
        // We would search the PATH environment variable for the command
        const char *pathEnv = getenv("PATH");
        char *pathCopy = strdup(pathEnv);
        char fullPath[1024];
        int found = 0;

        // Tokenize the PATH variable and check each directory for the executable
        char *dir = strtok(pathCopy, ";");
        while (dir != NULL)
        {
          // Strip trailing backslash from dir if present
          int len = strlen(dir);
          if (dir[len - 1] == '\\')
          dir[len - 1] = '\0';
          
          snprintf(fullPath, sizeof(fullPath), "%s\\%s.exe", dir, arg);

          // Check if fullpath is executable
          if (_access(fullPath, 0) == 0)
          {
            printf("%s is %s\n", arg, fullPath);
            found = 1;
            break;
          }

          dir = strtok(NULL, ";");
        }

        if (!found)
        {
          fprintf(stderr, "Error: '%s' is not executable or not found in PATH\n", arg);
        }

        free(pathCopy);
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
