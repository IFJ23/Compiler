#include "parser.h" // Include the header file of your code

int main() {
    // Run the parser with the input from stdin
    Scanner scanner;
    scanner.file = stdin; // Set input to stdin
    parserInit();
    int result = parse(&scanner);

    return result;
}