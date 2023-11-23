// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)

#include "parser.h" // Include the header file of your code

int main() {
    // Run the parser with the input from stdin
    Scanner scanner;
    scanner.file = stdin; // Set input to stdin
    scanner.line = 1; // Set line to 1
    parserInit();
    int result = parse(&scanner);

    // Check parsing result
    if (result != 0) {
        fprintf(stderr, "Parsing failed %d\n", result);
        return 1;
    }

    return 0;
}