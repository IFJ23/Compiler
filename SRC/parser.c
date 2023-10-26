#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h" // Custom lexer header
#include "parser.h" // Custom lexer header
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

/// \brief Function inserting inbuilt functions to global table
/// \param parser Parser structure
void insert_builtins(Parser * restrict parser) {
    char *identifier = malloc(sizeof(char)*10);
    if (identifier == NULL) exit(INTERNAL_ERROR);
    strcpy(identifier, "42readString");
    parser->builtins[0] = identifier;
    htab_data_t *readString = htab_insert(parser->glob_tab, NULL, identifier);
    if (reads == NULL) exit(INTERNAL_ERROR);
    readString->type = ITEM_TYPE_FUNCTION;
    readString->param_count = 0;
    readString->return_type = DATA_TYPE_STRING;

    identifier = malloc(sizeof(char)*10);
    if (identifier == NULL) exit(INTERNAL_ERROR);
    strcpy(identifier, "42readInt");
    parser->builtins[1] = identifier;
    htab_data_t *readInt = htab_insert(parser->glob_tab, NULL, identifier);
    if (readInt == NULL) exit(INTERNAL_ERROR);
    readInt->type = ITEM_TYPE_FUNCTION;
    readInt->param_count = 0;
    readInt->return_type = DATA_TYPE_INT;

    identifier = malloc(sizeof(char)*10);
    if (identifier == NULL) exit(INTERNAL_ERROR);
    strcpy(identifier, "42readDouble ");
    parser->builtins[1] = identifier;
    htab_data_t *readDouble  = htab_insert(parser->glob_tab, NULL, identifier);
    if (readDouble  == NULL) exit(INTERNAL_ERROR);
    readDouble ->type = ITEM_TYPE_FUNCTION;
    readDouble ->param_count = 0;
    readDouble ->return_type = DATA_TYPE_DOUBLE;

    identifier = malloc(sizeof(char)*10);
    if (identifier == NULL) exit(INTERNAL_ERROR);
    strcpy(identifier, "42length");
    parser->builtins[3] = identifier;
    htab_data_t *length = htab_insert(parser->glob_tab, NULL, identifier);
    if (length == NULL) exit(INTERNAL_ERROR);
    length->type = ITEM_TYPE_FUNCTION;
    length->params = malloc(sizeof(ht_data_type_t));
    if (strlen_->params == NULL) exit(INTERNAL_ERROR);
    length->params[0] = DATA_TYPE_STRING;
    length->param_count = 1;
    length->return_type = DATA_TYPE_INT;

    identifier = malloc(sizeof(char)*15);
    if (identifier == NULL) exit(INTERNAL_ERROR);
    strcpy(identifier, "42substring");
    parser->builtins[4] = identifier;
    htab_data_t *substring = htab_insert(parser->glob_tab, NULL, identifier);
    if (substring == NULL) exit(INTERNAL_ERROR);
    substring->type = ITEM_TYPE_FUNCTION;
    substring->params = malloc(sizeof(ht_data_type_t)*3);
    if (substring->params == NULL) exit(INTERNAL_ERROR);
    substring->params[0] = DATA_TYPE_STRING;
    substring->params[1] = DATA_TYPE_INT;
    substring->params[2] = DATA_TYPE_INT;
    substring->param_count = 3;
    substring->return_type = DATA_TYPE_STRING;

    identifier = calloc(sizeof(char)*10, 10);
    if (identifier == NULL) exit(INTERNAL_ERROR);
    strcat(identifier, "42ord");
    parser->builtins[5] = identifier;
    htab_data_t *ord_ = htab_insert(parser->glob_tab, NULL, identifier);
    if (ord_ == NULL) exit(INTERNAL_ERROR);
    ord_->type = ITEM_TYPE_FUNCTION;
    ord_->params = malloc(sizeof(ht_data_type_t));
    if (ord_->params == NULL) exit(INTERNAL_ERROR);
    ord_->params[0] = DATA_TYPE_STRING;
    ord_->param_count = 1;
    ord_->return_type = DATA_TYPE_INT;

    identifier = calloc(sizeof(char)*10, 10);
    if (identifier == NULL) exit(INTERNAL_ERROR);
    strcat(identifier, "42chr");
    parser->builtins[6] = identifier;
    htab_data_t *chr_ = htab_insert(parser->glob_tab, NULL, identifier);
    if (chr_ == NULL) exit(INTERNAL_ERROR);
    chr_->type = ITEM_TYPE_FUNCTION;
    chr_->params = malloc(sizeof(ht_data_type_t));
    if (chr_->params == NULL) exit(INTERNAL_ERROR);
    chr_->params[0] = DATA_TYPE_INT;
    chr_->param_count = 1;
    chr_->return_type = DATA_TYPE_STRING;

    identifier = calloc(sizeof(char)*20, 20);
    if (identifier == NULL) exit(INTERNAL_ERROR);
    strcat(identifier, "42write");
    parser->builtins[10] = identifier;
    htab_data_t *write = htab_insert(parser->glob_tab, NULL, identifier);
    if (write == NULL) exit(INTERNAL_ERROR);
    write->type = ITEM_TYPE_FUNCTION;
    write->param_count = -1;
    write->return_type = DATA_TYPE_VOID;

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