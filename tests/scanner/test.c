//
// Created by Dews on 15.10.2023.

// Nothing is here. Just for github
//

#include <stdio.h>
#include <assert.h>
#include "../../SRC/scanner.h" // Include the header file of your code

// Define a helper function to run tests
void run_test(const char *test_name, Token_type expected_type, int expected_line, Scanner scanner) {

    Token token;

    printf("Running test: %s\n", test_name);

    int result = get_token(&scanner, &token);
    
    assert(result == EXIT_SUCCESS);
    printf("Token type: %d\n", token.type);
    assert(token.type == expected_type);
    printf("Token line: %d\n", token.line);
    //assert(token.line == expected_line);
}

int main() {
    // Create input files for your test cases
    FILE *test_input = fopen("test_cases.txt", "r");
    Scanner scanner;
    scanner.file = test_input;
    scanner.line = 1;
    // Test 1: Check for TYPE_PLUS
    // Test all the cases from test_cases.txt
    run_test("Test for TYPE_PLUS", TYPE_PLUS, 2, scanner);
    run_test("Test for TYPE_MINUS", TYPE_MINUS, 3, scanner);
    run_test("Test for TYPE_MUL", TYPE_MUL, 4 , scanner);
    run_test("Test for TYPE_DIV", TYPE_DIV, 5, scanner);
    run_test("Test for TYPE_LESS", TYPE_LESS, 6 , scanner);
    run_test("Test for TYPE_LESS_EQUAL", TYPE_LESS_EQUAL, 7 , scanner );
    run_test("Test for TYPE_MORE", TYPE_MORE, 8, scanner);
    run_test("Test for TYPE_MORE_EQUAL", TYPE_MORE_EQUAL, 9, scanner);
    run_test("Test for TYPE_ASSIGN", TYPE_ASSIGN, 10, scanner);
    run_test("Test for TYPE_EQUAL", TYPE_EQUAL, 11, scanner);
    run_test("Test for TYPE_NOT_EQUAL", TYPE_NOT_EQUAL, 12, scanner);
    run_test("Test for TYPE_LEFT_BRACKET", TYPE_LEFT_BRACKET, 13, scanner);
    run_test("Test for TYPE_RIGHT_BRACKET", TYPE_RIGHT_BRACKET, 14, scanner);
    run_test("Test for TYPE_COMMA", TYPE_COMMA, 16, scanner);
    run_test("Test for TYPE_COLON", TYPE_COLON, 17, scanner);
    run_test("Test for TYPE_LEFT_CURLY_BRACKET", TYPE_LEFT_CURLY_BRACKET, 18, scanner);
    run_test("Test for TYPE_RIGHT_CURLY_BRACKET", TYPE_RIGHT_CURLY_BRACKET, 19, scanner);
    run_test("Test for TYPE_NIL_COALESCING_OPERATOR", TYPE_NIL_COALESCING_OPERATOR, 20, scanner);
    run_test("Test for TYPE_ID_VAR", TYPE_IDENTIFIER_VAR, 21, scanner);
    run_test("Test for TYPE_KW (Keyword)", TYPE_KW, 22, scanner);
    run_test("Test for TYPE_KW (Keyword)", TYPE_KW, 23, scanner);
    run_test("Test for TYPE_STRING", TYPE_STRING, 24, scanner);
    run_test("Test for TYPE_INT", TYPE_INT, 25, scanner);
    run_test("Test for TYPE_DOUBLE", TYPE_DOUBLE, 26, scanner);
    run_test("Test for TYPE_MULTILINE_STRING", TYPE_MULTILINE_STRING, 27, scanner);


    // Add more test cases here for other token types, keywords, and identifiers

    // Close input files
    fclose(test_input);

    printf("All tests passed!\n");
    return 0;
}