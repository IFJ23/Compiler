// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Ivan Onufriienko (xonufr00)

#ifndef IFJ23_SCANNER_H
#define IFJ23_SCANNER_H

#include "errors.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
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
    KW_WHILE,
    KW_INT_NIL,
    KW_DOUBLE_NIL,
    KW_STRING_NIL,
} Keyword;

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
    TYPE_RETURN_ARROW,
    TYPE_NIL_COALESCING_OPERATOR,
    TYPE_MULTILINE_STRING,
    TYPE_ERROR,
} Token_type;

typedef union {
    Keyword kw;
    char *id;
    int integer;
    double decimal;
    char *string;
} Token_value;
typedef struct {
    Token_type type;
    Token_value value;
    int line;
} Token;

typedef struct{
    FILE *file;
    int line;
} Scanner;

int get_token(Scanner*, Token*);

int keyword_from_token(Token*, char*);
#endif //IFJ23_SCANNER_H