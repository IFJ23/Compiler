// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Ivan Onufriienko (xonufr00)

#ifndef IFJ23_SCANNER_H 
#define IFJ23_SCANNER_H

#include "error.h"
#include <stdlib.h>
#include <stdio.h>

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
} Token_type;


#endif //IFJ23_SCANNER_H