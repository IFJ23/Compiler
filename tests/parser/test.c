#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../../SRC/parser.h" // Include the header file of your code
#include "../../SRC/scanner.h" // Include the header file of your code

int main() {
    // Run the parser with the test file
    FILE *file = fopen("test_content.txt", "r");
    if (file == NULL) {
        printf("Unable to open the test file\n");
        return 1;
    }

    Scanner scanner;
    scanner.file = file;

    int result = parse(&scanner);

    // Add your own assertions for testing
    if (result != 0) {
        printf("Parsing failed\n");
        return 1;
    }

    printf("Parsing successful\n");

    fclose(file);
    return 0;
}