// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file error.c
 * @brief Error codes and handling for the IFJ23 language compiler
 * @author Ivan Onufriienko (xonufr00)
 */

#include <stdio.h>
#include "error.h"

void printError(int line, char *message)
{
    if (line > 0)
        fprintf(stderr, "%d\tError: %s\n", line, message);
    else
        fprintf(stderr, "Error: %s\n", message);
}
