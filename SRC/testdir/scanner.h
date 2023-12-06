// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file scanner.h
 * @brief Compiler to IFJ23 language
 * @author Ivan Onufriienko (xonufr00)
 */

#ifndef IFJ23_SCANNER_H
#define IFJ23_SCANNER_H

#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

/**
 * @brief Enum representing keywords in the IFJ23 language.
 */
typedef enum {
    KW_DOUBLE,
    KW_ELSE,
    KW_FUNC,
    KW_IF,
    KW_INT,
    KW_LET,
    KW_NIL,
    KW_RETURN,
    KW_STRING,
    KW_VAR,
    KW_VOID,
    KW_WHILE,
    KW_UNDEFINED_INT,
    KW_UNDEFINED_DOUBLE,
    KW_UNDEFINED_STRING,
} Keyword;

/**
 * @brief Enum representing types of tokens in the IFJ23 language.
 */
typedef enum {
    TYPE_KW,
    TYPE_ID,
    TYPE_EOL,
    TYPE_EOF,
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_PLUS,
    TYPE_MINUS,
    TYPE_MUL,
    TYPE_DIV,
    TYPE_LESS,
    TYPE_MORE,
    TYPE_LESS_EQUAL,
    TYPE_MORE_EQUAL,
    TYPE_EQUAL,
    TYPE_NOT_EQUAL,
    TYPE_ASSIGN,
    TYPE_LEFT_BRACKET,
    TYPE_RIGHT_BRACKET,
    TYPE_COMMA,
    TYPE_COLON,
    TYPE_EXCLAMATION_MARK,
    TYPE_LEFT_CURLY_BRACKET,
    TYPE_RIGHT_CURLY_BRACKET,
    TYPE_IDENTIFIER_FUNC,
    TYPE_IDENTIFIER_VAR,
    TYPE_OPTIONAL_TYPE,
    TYPE_RETURN_ARROW,
    TYPE_NIL_COALESCING_OPERATOR,
    TYPE_MULTILINE_STRING,
    TYPE_ERROR,
} Token_type;

/**
 * @brief Union representing the value of a token in the IFJ23 language.
 * @param kw What keyword the token is.
 * @param id Identifier value of the token.
 * @param integer Integer value of the token.
 * @param decimal Decimal value of the token.
 * @param string String value of the token.
 */
typedef union {
    Keyword kw;
    char *id;
    int integer;
    double decimal;
    char *string;
} Token_value;

/**
 * @brief Struct representing a token in the IFJ23 language.
 * @param type Type of the token.
 * @param value Value of the token.
 * @param line Line where the token is.
 */
typedef struct {
    Token_type type;
    Token_value value;
    int line;
} Token;


/**
 * @brief Struct representing a scanner in the IFJ23 language.
 * @param file File to get the tokens from.
 * @param line Current line in the file.
 */
typedef struct{
    FILE *file;
    int line;
} Scanner;

/**
 * @brief Function to get a token from the scanner.
 * @param scanner The scanner to get the token from.
 * @param token The token to be filled with the data.
 * @return 0 if successful, non-zero otherwise.
 */
int get_token(Scanner*, Token*);

/**
 * @brief Function to get a keyword from a token.
 * @param token The token to get the keyword from.
 * @param char The string to compare with the keyword.
 * @return 0 if successful, non-zero otherwise.
 */
int keyword_from_token(Token*, char*);

/**
 * @brief Function to peek at the next token from the scanner.
 * @param scanner The scanner to peek the token from.
 * @param token The token to be filled with the data.
 * @return 0 if successful, non-zero otherwise.
 */
int peek_token(Scanner*, Token*);

#endif //IFJ23_SCANNER_H
