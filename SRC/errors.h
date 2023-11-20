// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Ivan Onufriienko (xonufr00)

#ifndef IFJ23_ERROR_H
#define IFJ23_ERROR_H

#define LEXICAL_ERROR 1                 // incorrect structure of the current lexeme

#define SYNTAX_ERROR 2                  // incorrect syntax of the program, missing header, etc.

#define SEMANTIC_DEFINITION_ERROR 3     // undefined function, variable redefinition

#define SEMANTIC_TYPE_ERROR 4           // wrong number/type of parameters when calling the function or 
                                        // wrong type of return value from function

#define SEMANTIC_UNDEFINED_ERROR 5      // use of an undefined or uninitialized variable

#define SEMANTIC_RETURN_ERROR 6         // missing/remaining expression in the return command from the function

#define SEMANTIC_COMPATIBILITY_ERROR 7  // compatibility error in arithmetic, string and relational expressions

#define SEMANTIC_UNDECLARED_ERROR 8     // inference error - variable or parameter type is not specified and 
                                        // cannot be inferred from the expression used

#define SEMANTIC_OTHER_ERROR 9          // other semantic errors

#define INTERNAL_ERROR 99               // internal compiler error


#endif //IFJ23_ERROR_H
