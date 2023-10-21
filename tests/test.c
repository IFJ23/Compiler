//
// Created by Dews on 15.10.2023.

// Nothing is here. Just for github
//

#include <stdio.h>
#include <assert.h>
#include "../SRC/scanner.h" // Include the header file of your code

// Define a helper function to run tests
void run_test(const char *test_name, Token_type expected_type, int expected_line, FILE *input_file) {
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
    FILE *test_input = fopen("test_cases.txt", "r");

    // Test 1: Check for TYPE_PLUS
    // Test all the cases from test_cases.txt
    run_test("Test for TYPE_PLUS", TYPE_PLUS, 0, test_input);
    run_test("Test for TYPE_MINUS", TYPE_MINUS, 0, test_input);
    run_test("Test for TYPE_MUL", TYPE_MUL, 0, test_input);
    run_test("Test for TYPE_DIV", TYPE_DIV, 0, test_input);
    run_test("Test for TYPE_LESS", TYPE_LESS, 0, test_input);
    run_test("Test for TYPE_LESS_EQUAL", TYPE_LESS_EQUAL, 0, test_input);
    run_test("Test for TYPE_MORE", TYPE_MORE, 0, test_input);
    run_test("Test for TYPE_MORE_EQUAL", TYPE_MORE_EQUAL, 0, test_input);
    run_test("Test for TYPE_ASSIGN", TYPE_ASSIGN, 0, test_input);
    run_test("Test for TYPE_EQUAL", TYPE_EQUAL, 0, test_input);
    run_test("Test for TYPE_NOT_EQUAL", TYPE_NOT_EQUAL, 0, test_input);
    run_test("Test for TYPE_LEFT_BRACKET", TYPE_LEFT_BRACKET, 0, test_input);
    run_test("Test for TYPE_RIGHT_BRACKET", TYPE_RIGHT_BRACKET, 0, test_input);
    run_test("Test for TYPE_COMMA", TYPE_COMMA, 0, test_input);
    run_test("Test for TYPE_COLON", TYPE_COLON, 0, test_input);
    run_test("Test for TYPE_LEFT_CURLY_BRACKET", TYPE_LEFT_CURLY_BRACKET, 0, test_input);
    run_test("Test for TYPE_RIGHT_CURLY_BRACKET", TYPE_RIGHT_CURLY_BRACKET, 0, test_input);
    run_test("Test for TYPE_NIL_COALESCING_OPERATOR", TYPE_NIL_COALESCING_OPERATOR, 0, test_input);
    run_test("Test for TYPE_ID", TYPE_ID, 0, test_input);
    run_test("Test for TYPE_KW (Keyword)", TYPE_KW, 0, test_input);
    run_test("Test for TYPE_KW (Keyword)", TYPE_KW, 0, test_input);
    run_test("Test for TYPE_STRING", TYPE_STRING, 0, test_input);
    run_test("Test for TYPE_INT", TYPE_INT, 0, test_input);
    run_test("Test for TYPE_DOUBLE", TYPE_DOUBLE, 0, test_input);


    // Add more test cases here for other token types, keywords, and identifiers

    // Close input files
    fclose(test_input);

    printf("All tests passed!\n");
    return 0;
}