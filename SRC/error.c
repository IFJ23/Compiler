#include <stdio.h>
#include "error.h"

void printError(int line, char *message)
{
    if (line > 0)
        fprintf(stderr, "%d\tError: %s\n", line, message);
    else
        fprintf(stderr, "Error: %s\n", message);
}
