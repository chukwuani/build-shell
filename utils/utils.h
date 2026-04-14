#ifndef UTILS_H
#define UTILS_H

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

void trimTrailing(char *str);
char *locateExecFile(const char *name);
void execute(char **args);
int parseArgs(char *input, char **args);

// Returns 1 if redirection was handled, 0 if not, -1 on error
int handleRedirection(char **args);

#endif