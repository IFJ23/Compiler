// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Ivan Onufriienko (xonufr00)
// Vsevolod Pokhvalenko (xpokhv00)

#include <stdio.h>
#include "error.h"

void printError(int line, char *errorText)
{
    if (line > 0)
        fprintf(stderr, "%d\tError: %s\n", line, errorText);
    else
        fprintf(stderr, "Error: %s\n", errorText);
}
