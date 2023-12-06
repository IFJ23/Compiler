// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file main.c
 * @brief Main file for the IFJ23 language compiler
 * @author Vsevolod Pokhvalenko (xpokhv00)
 */

#include "parser.h" // Include the header file of your code
/**
 * @brief Main function
 * @return 0 if everything went well, 1 if there was an error
*/
int main() {
    // Run the parser with the input from stdin
    Scanner scanner;
    scanner.file = stdin; // Set input to stdin
    parserInit();
    int result = parse(&scanner);

    return result;
}