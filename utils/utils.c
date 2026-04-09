#include "utils.h"
#include <ctype.h>
#include <string.h>

void trimTrailing(char *str){
    int len = strlen(str);

    while (len > 0 && isspace((unsigned char)str[len - 1]))
    {
        str[--len] = '\0';
    }
}