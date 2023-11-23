
#include "parser.h" // Include the header file of your code

int main() {
    // Run the parser with the input from stdin
    Scanner scanner;
    scanner.file = stdin; // Set input to stdin
    parserInit();
    int result = parse(&scanner);

    // Check parsing result
    if (result != 0) {
        fprintf(stderr, "Parsing failed %d\n", result);
        return 1;
    }

    return 0;
}