#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h" // Custom lexer header
#include "stdarg.h" // Custom lexer header
#include "ast.h"   // Custom AST header
#include "error.h" // Custom error handling header

typedef struct parser {
    // Add fields for your parser's state and information
    token_t *curr_func_id;
    size_t decl_cnt;
    size_t cond_cnt;
    size_t loop_cnt;
    bool found_return;
    tok_stack_t decl_func;
    int return_code;
    bool reached_EOF;
    Scanner *scanner;
    symbol_tables_t sym;
    prog_t dst_code;
} parser_t;

int parser_init(parser_t *parser, Scanner *scanner) {
    // Initialize your parser fields, e.g., counters, flags, stacks, etc.
    parser->curr_func_id = NULL;
    parser->decl_cnt = 0;
    parser->cond_cnt = 0;
    parser->loop_cnt = 0;
    parser->found_return = false;
    parser->return_code = PARSE_SUCCESS;
    parser->reached_EOF = false;
    parser->scanner = scanner;

    // Initialize other components like symbol tables, destination code, etc.
    // You may need to allocate memory and initialize them.

    return EXIT_SUCCESS;
}

void parser_cleanup(parser_t *parser) {
    // Release any dynamically allocated memory, close files, and clean up resources.
    // For example, release the memory used by symbol tables and destination code.

    // Optionally, free any remaining memory in your parser structure.

    // Close files or other resources.

    // Finally, free the parser structure itself.
    free(parser);
}

int parse_program(parser_t *parser) {
    // Parse the program's grammar, starting with the entry point.
    // Build the AST as you parse the code.

    // Return the appropriate return code.
}

void error(parser_t *parser, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    // Set the appropriate error code in the parser structure.
}

typedef struct ast_node {
    // Define fields for the AST node, e.g., type, value, children, etc.
} ast_node_t;

ast_node_t *create_ast_node(/* arguments */) {
    // Create a new AST node, set its fields, and return it.
}

int perform_semantic_analysis(parser_t *parser, ast_node_t *ast) {
    // Implement semantic analysis rules.
    // Report semantic errors if any are found.
}


/// \param value unsigned int value
/// \param type unsigned int type
/// \return Pointer to allocated memory
// maybe will be needed
TData *stack_data(int value, int type) {
    TData *ptr = malloc(sizeof(TData));
    if (ptr != NULL) {
        ptr->value = value;
        ptr->type = type;
        ptr->htab = NULL;
        ptr->bucket = NULL;
    } else {
        exit(INTERNAL_ERROR);
    }
    return ptr;
}