//
// Created by Dews on 15.10.2023.

// Nothing is here. Just for github
//

#include <stdio.h>
#include <assert.h>
#include "../SRC/scanner.h" // Include the header file of your code

// Define a helper function to run tests
void run_test(const char *test_name, int expected_type, int expected_line, FILE *input_file) {
    Scanner scanner;
    scanner.file = input_file;
    Token token;

    printf("Running test: %s\n", test_name);

    int result = get_token(&scanner, &token);
    assert(result == EXIT_SUCCESS);
    assert(token.type == expected_type);
    assert(token.line == expected_line);
}

int main() {
    // Create input files for your test cases
    FILE *test_input1 = fopen("test1.txt", "r");
    FILE *test_input2 = fopen("test2.txt", "r");

    // Test 1: Check for TYPE_PLUS
    run_test("Test for TYPE_PLUS", TYPE_PLUS, 1, test_input1);

    // Test 2: Check for TYPE_RETURN_ARROW
    run_test("Test for TYPE_RETURN_ARROW", TYPE_RETURN_ARROW, 2, test_input2);

    // Add more test cases here for other token types, keywords, and identifiers

    // Close input files
    fclose(test_input1);
    fclose(test_input2);

    printf("All tests passed!\n");
    return 0;
}
