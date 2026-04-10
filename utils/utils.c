#include "utils.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>  
#include <unistd.h>  

void trimTrailing(char *str)
{
    int len = strlen(str);

    while (len > 0 && isspace((unsigned char)str[len - 1]))
    {
        str[--len] = '\0';
    }
}

char *locateExecFile(char *arg)
{
    char *pathEnv = getenv("PATH");
    char *pathCopy = strdup(pathEnv);

    char fullPath[1024];
    int found = 0;

    char *dir = strtok(pathCopy, ":");
    while (dir != NULL)
    {
        // Strip trailing backslash from dir if present
        // int len = strlen(dir);
        // if (dir[len - 1] == '\\')
        //     dir[len - 1] = '\0';

        // Construct fullpath
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, arg);

        // Check if it's an exec file
        if (access(fullPath, X_OK) == 0)
        {
            found = 1;
            break;
        }

        dir = strtok(NULL, ":");
    }

    free(pathCopy);

    // I used found previous but I also need the fullpath
    // Return fullpath if exec and empty string if not
    return found ? fullPath : "";
}

void execute(char **args)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        // Inside child process — run the command
        execvp(args[0], args);

        // execvp only returns if it FAILED
        fprintf(stderr, "Error: command not found: %s\n", args[0]);
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // Inside parent process — wait for child to finish
        wait(NULL);
    }
    else
    {
        fprintf(stderr, "Error: fork failed\n");
    }
}