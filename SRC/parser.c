// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h" // Custom lexer header
#include "parser.h" // Custom lexer header
#include "stdarg.h" // Custom lexer header
#include "ast.h"   // Custom AST header
#include "error.h" // Custom error handling header

#define LEXICAL_OK 0


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

/// \brief Gets row and column indexes for the precedence table
/// \param token Last token read
/// \param rc Row or column index
/// \param symbol Specified symbol from the table
void prec_index(const Token * restrict token, unsigned int * restrict rc, int symbol) {
    if (symbol > -1 && symbol < 15 && token == NULL) {
        *rc = symbol;
        return;
    } else if (token == NULL) {
        exit(INTERNAL_ERROR);
    }
    switch (token->type) {

        case T_IDENTIFIER:
            *rc = 12;
            break;
        case TYPE_STRING: case TYPE_INT: case TYPE_FLOAT: case TYPE_VAR:
            *rc = 5;
            break;
        case TYPE_COMMA:
            *rc = 13;
            break;
        case TYPE_EOF: case T_END:
            exit(SYNTAX_ERROR);
        case TYPE_PLUS:
            *rc = 2;
            break;
        case TYPE_MINUS:
            *rc = 3;
            break;
        case TYPE_MULTILINE_STRING:
            *rc = 4;
            break;
        case TYPE_MUL:
            *rc = 0;
            break;
        case TYPE_DIV:
            *rc = 1;
            break;
        case TYPE_LESS: case TYPE_LESS_EQUAL: case TYPE_MORE: case TYPE_MORE_EQUAL:
            *rc = 8;
            break;
        case TYPE_EQUAL:
            *rc = 9;
            break;
        case TYPE_NOT_EQUAL:
            *rc = 10;
            break;
        case TYPE_LEFT_BRACKET:
            *rc = 6;
            break;
        case TYPE_RIGHT_BRACKET:
            *rc = 7;
            break;
        case TYPE_ERROR:
            exit(LEXEME_ERROR);
        case TYPE_LEFT_CURLY_BRACKET:
            break;
        case TYPE_KW:
            if (token->value.keyword == KW_NULL) {
                *rc = 5;
            } else {
                exit(SYNTAX_ERROR);
            }
            break;
        default:
            exit(SYNTAX_ERROR);
    }
}

/// \brief Expression parser function for reduction rules
/// \param stack Precedence stack
/// \param shelf Supportive stack
/// \param temps Stack storing results of expressions
/// \param gen Generator structure
/// \param end indicates whether the end of the expression has been reached
/// \param parser Parser structure
/// \return -1 on empty expression, 0 on error, 1 on success
int reduce(register TStack * restrict stack, register TStack * restrict shelf, TStack * restrict temps,
           register Generator * restrict gen, bool end, Parser * restrict parser) {

    if (stack_top(stack)->value == P_E) {
        stack_push(shelf, stack_pop(stack));
        if (stack_top(stack)->value == P_END) {
            stack_push(stack, stack_pop(shelf));
            return -1; //empty expression
        }
    }
    unsigned int res = 0;
    bool fn = false;
    register htab_data_t *last_fn = NULL;

    while (stack_top(stack)->value != P_OPEN && stack_top(stack)->value != P_END) {
        TData *data = stack_pop(stack);
        stack_push(shelf, data);
        res += data->value;
        if (data->value == P_FN) {
            fn = true;
            last_fn = data->bucket;
        }
    }
    if (stack_top(stack)->value == P_OPEN) {
        const TData *data = stack_pop(stack);
        res += data->value;
        stack_push(shelf, data);
    } else if (stack_top(stack)->value == P_END && end) {
        return -1;
    } else if (stack_top(stack)->value == P_END && !end) {
        exit(SYNTAX_ERROR);
    }

    int cnt = 0;
    TData *op_one;
    TData *op_two;
    htab_data_t **operands = NULL;

    size_t alloc_num = snprintf(NULL, 0, "%d", parser->tmp_counter) + 1;
    char *number = malloc(alloc_num);
    if (number == NULL) exit(INTERNAL_ERROR);
    snprintf(number, alloc_num, "%d", parser->tmp_counter);

    char *tmp = malloc(sizeof(char) * (TEMP_LENGTH + alloc_num));
    if (tmp == NULL) exit(INTERNAL_ERROR);
    strncpy(tmp, TEMP_VAR_PREFIX, TEMP_LENGTH);
    strncat(tmp, number, sizeof(char) * (TEMP_LENGTH + alloc_num));

    free(number);

    TData *data = NULL;
    htab_data_t *htab_data = NULL;
    if (res > 34 && res != 59) {
        parser->tmp_counter++;
        htab_data = htab_insert(parser->temporary_tab, NULL, tmp);
        data = stack_data(P_E, P_E);
        data->bucket = htab_data;
        data->bucket->type = H_VAR;
    } else {
        free(tmp);
    }

    if (fn) {
        if (data == NULL) {
            exit(SYNTAX_ERROR);
        }
        data->bucket->type = H_FUNC_ID;
        //symtable is used here to check for number of args
        int brackets = 0;
        int E = 0;
        int commas = 0;
        int previous_args = 0;
        TStack *reversal = NULL;
        reversal = stack_init(reversal);

        if (last_fn->return_type == D_NONE && last_fn->param_count != 0) {
            previous_args = last_fn->param_count;
            last_fn->param_count = 0;
        }

        bool expect_comma = false;

        while (stack_top(shelf)->value != P_CLOSE) {
            TData *tmp_data = stack_pop(shelf);
            stack_push(parser->garbage_bin, tmp_data);
            if (tmp_data->value == P_LEFT_BRACKET) brackets--;
            else if (tmp_data->value == P_RIGHT_BRACKET) brackets++;
            else if (tmp_data->value == P_E) {
                E++;
                expect_comma = true;

                tmp_data = stack_pop(temps);
                stack_push(reversal, tmp_data);
                if (last_fn->return_type == D_NONE) {
                    last_fn->params = realloc(last_fn->params, sizeof(DataType) * E);
                    for (int j = E - 1; j > 0; j--) {
                        last_fn->params[j] = last_fn->params[j - 1];
                    }
                    last_fn->params[0] = tmp_data->bucket->value_type;
                    last_fn->params_strict = realloc(last_fn->params_strict, sizeof(DataType) * E);
                    last_fn->params_strict[last_fn->param_count] = true;
                    if (tmp_data->bucket->value_type == D_VOID) {
                        last_fn->params_strict[last_fn->param_count] = false;
                    }
                    last_fn->param_count++;
                }
            } else if (tmp_data->value == P_COMMA) {
                if (!expect_comma) exit(SYNTAX_ERROR);
                commas++;
                expect_comma = false;
            }
        }
        if (brackets != 0) exit(SYNTAX_ERROR);
        if (stack_top(shelf) != NULL) { // pops last bracket
            stack_push(parser->garbage_bin, stack_pop(shelf));
        }

        // args number check
        if (E != last_fn->param_count && last_fn->param_count != -1 && last_fn->return_type != D_NONE)  {
            exit(PARAMETER_OR_RETURN_ERROR);
        }
        if (E != previous_args && previous_args != 0) {
            exit(PARAMETER_OR_RETURN_ERROR);
        }

        if (commas != E - 1 && E != 0) exit(SYNTAX_ERROR);

        int builtin = -1;
        for (int j = 0; j < 11; j++) {
            if(strcmp(last_fn->identifier, parser->builtins[j]) == 0) {
                builtin = j;
                break;
            }
        }

        htab_data_t **params = NULL;
        if (builtin != -1) {
            data->bucket->type = H_VAR;
            defvar_order(tmp, htab_data, gen, parser);

            params = E ? malloc(sizeof(htab_data_t*) * E) : malloc(sizeof(htab_data_t*));
            for (int j = 0; j < E; j++) {
                TData *garbage = stack_pop(reversal);
                params[j] = garbage->bucket;
                stack_push(parser->garbage_bin, garbage);
            }

            generator_add_instruction(gen, gen_instruction_constructor(builtin, tmp, operands, NULL, 0, params, E));
        } else {
            // creates a temporary variable
            defvar_order(tmp, htab_data, gen, parser);

            // prepares the call instruction
            operands = malloc(sizeof(htab_data_t *));
            operands[0] = last_fn;
            params = malloc(sizeof(htab_data_t*) * last_fn->param_count);
            for (int i = 0; i < last_fn->param_count; i++) {
                TData *garbage = stack_pop(reversal);
                params[i] = garbage->bucket;
                stack_push(parser->garbage_bin, garbage);
            }
            generator_add_instruction(gen, gen_instruction_constructor(call, tmp, operands, last_fn->params, 1, params, last_fn->param_count));
        }

        printf("11p ");
        stack_push(stack, stack_data(P_E, P_E));
        data->bucket->value_type = last_fn->return_type;
        data->bucket->return_type = last_fn->return_type;
        stack_push(temps, data);
        stack_free(reversal);
        return 1;
    }

    switch (res) {
        case 34: // <i>
            while (cnt < 3) {
                const TData *garbage = stack_pop(shelf);
                if (garbage == NULL) {
                    break;
                }
                stack_push(parser->garbage_bin, garbage);
                cnt++;
            }
            if (cnt != 3) break;
            stack_push(stack, stack_data(P_E, P_E));
            printf("1p ");
            return 1;
        case 54:
            exit(SYNTAX_ERROR);
        case 63: // multiplication
            while (cnt < 5) {
                const TData *garbage = stack_pop(shelf);
                if (garbage == NULL) {
                    break;
                }
                stack_push(parser->garbage_bin, garbage);
                cnt++;
            }
            if (cnt != 5) break;
            op_one = stack_pop(temps);
            op_two = stack_pop(temps);

            // type of the outcome
            if (op_one->bucket->value_type == D_FLOAT || op_two->bucket->value_type == D_FLOAT) {
                htab_data->value_type = D_FLOAT;
            } else if (op_one->bucket->value_type == D_INT || op_two->bucket->value_type == D_INT) {
                htab_data->value_type = D_INT;
            } else if (op_one->bucket->value_type == D_VOID && op_two->bucket->value_type == D_VOID) {
                htab_data->value_type = D_INT;
            } else {
                htab_data->value_type = D_VOID;
            }

            defvar_order(tmp, htab_data, gen, parser);

            operands = malloc(sizeof(htab_data_t*) * 2);
            operands[0] = op_one->bucket; // order doesn't matter here
            operands[1] = op_two->bucket;
            generator_add_instruction(gen, gen_instruction_constructor(mul, tmp, operands, NULL, 2, NULL, 0));

            stack_push(parser->garbage_bin, op_one);
            stack_push(parser->garbage_bin, op_two);

            printf("2p ");
            stack_push(temps, data);
            stack_push(stack, stack_data(P_E, P_E));
            return 1;
        case 64: // division
            while (cnt < 5) {
                const TData *garbage = stack_pop(shelf);
                if (garbage == NULL) {
                    break;
                }
                stack_push(parser->garbage_bin, garbage);
                cnt++;
            }
            if (cnt != 5) break;
            op_one = stack_pop(temps);
            op_two = stack_pop(temps);
            htab_data->value_type = D_FLOAT;

            defvar_order(tmp, htab_data, gen, parser);

            operands = malloc(sizeof(htab_data_t*) * 2);
            operands[1] = op_one->bucket; //reversed due to stack
            operands[0] = op_two->bucket;
            generator_add_instruction(gen, gen_instruction_constructor(div_, tmp, operands, NULL, 2, NULL, 0));

            stack_push(parser->garbage_bin, op_one);
            stack_push(parser->garbage_bin, op_two);

            printf("3p ");
            stack_push(temps, data);
            stack_push(stack, stack_data(P_E, P_E));
            return 1;
        case 65: // addition
            while (cnt < 5) {
                const TData *garbage = stack_pop(shelf);
                if (garbage == NULL) {
                    break;
                }
                stack_push(parser->garbage_bin, garbage);
                cnt++;
            }
            if (cnt != 5) break;
            op_one = stack_pop(temps);
            op_two = stack_pop(temps);

            // type of the outcome
            if (op_one->bucket->value_type == D_FLOAT || op_two->bucket->value_type == D_FLOAT) {
                htab_data->value_type = D_FLOAT;
            } else if (op_one->bucket->value_type == D_INT || op_two->bucket->value_type == D_INT) {
                htab_data->value_type = D_INT;
            } else if (op_one->bucket->value_type == D_VOID && op_two->bucket->value_type == D_VOID) {
                htab_data->value_type = D_INT;
            } else {
                htab_data->value_type = D_VOID;
            }

            defvar_order(tmp, htab_data, gen, parser);

            operands = malloc(sizeof(htab_data_t*) * 2);
            operands[1] = op_one->bucket; //reversed due to stack
            operands[0] = op_two->bucket;
            generator_add_instruction(gen, gen_instruction_constructor(addition, tmp, operands, NULL, 2, NULL, 0));

            stack_push(parser->garbage_bin, op_one);
            stack_push(parser->garbage_bin, op_two);

            printf("4p ");
            stack_push(temps, data);
            stack_push(stack, stack_data(P_E, P_E));
            return 1;
        case 66: // subtraction
            while (cnt < 5) {
                const TData *garbage = stack_pop(shelf);
                if (garbage == NULL) {
                    break;
                }
                stack_push(parser->garbage_bin, garbage);
                cnt++;
            }
            if (cnt != 5) break;
            op_one = stack_pop(temps);
            op_two = stack_pop(temps);

            // type of the outcome
            if (op_one->bucket->value_type == D_FLOAT || op_two->bucket->value_type == D_FLOAT) {
                htab_data->value_type = D_FLOAT;
            } else if (op_one->bucket->value_type == D_INT || op_two->bucket->value_type == D_INT) {
                htab_data->value_type = D_INT;
            } else if (op_one->bucket->value_type == D_VOID && op_two->bucket->value_type == D_VOID) {
                htab_data->value_type = D_INT;
            } else {
                htab_data->value_type = D_VOID;
            }

            defvar_order(tmp, htab_data, gen, parser);

            operands = malloc(sizeof(htab_data_t*) * 2);
            operands[1] = op_one->bucket; //reversed due to stack
            operands[0] = op_two->bucket;
            generator_add_instruction(gen, gen_instruction_constructor(sub, tmp, operands, NULL, 2, NULL, 0));

            stack_push(parser->garbage_bin, op_one);
            stack_push(parser->garbage_bin, op_two);

            printf("5p ");
            stack_push(temps, data);
            stack_push(stack, stack_data(P_E, P_E));
            return 1;
        case 67: // concatenation
            while (cnt < 5) {
                const TData *garbage = stack_pop(shelf);
                if (garbage == NULL) {
                    break;
                }
                stack_push(parser->garbage_bin, garbage);
                cnt++;
            }
            if (cnt != 5) break;
            op_one = stack_pop(temps);
            op_two = stack_pop(temps);

            defvar_order(tmp, htab_data, gen, parser);

            operands = malloc(sizeof(htab_data_t*) * 2);
            operands[1] = op_one->bucket; //reversed due to stack
            operands[0] = op_two->bucket;
            generator_add_instruction(gen, gen_instruction_constructor(concat, tmp, operands, NULL, 2, NULL, 0));

            stack_push(parser->garbage_bin, op_one);
            stack_push(parser->garbage_bin, op_two);

            htab_data->value_type = D_STRING;

            printf("6p ");
            stack_push(temps, data);
            stack_push(stack, stack_data(P_E, P_E));
            return 1;
        case 59: // brackets
            while (cnt < 5) {
                const TData *garbage = stack_pop(shelf);
                if (garbage == NULL) {
                    break;
                }
                stack_push(parser->garbage_bin, garbage);
                cnt++;
            }
            if (cnt != 5) break;
            printf("7p ");
            stack_push(stack, stack_data(P_E, P_E));
            return 1;
        case 71: // relations or a function
            while (cnt < 5) {
                const TData *garbage = stack_pop(shelf);
                if (garbage == NULL) {
                    break;
                }
                stack_push(parser->garbage_bin, garbage);
                cnt++;
            }
            if (cnt != 5) break;
            op_one = stack_pop(temps);
            op_two = stack_pop(temps);
            htab_data->value_type = D_BOOL;

            defvar_order(tmp, htab_data, gen, parser);

            operands = malloc(sizeof(htab_data_t*) * 2);
            operands[1] = op_one->bucket; //reversed due to stack
            operands[0] = op_two->bucket;
            Instruction *rel = gen_instruction_constructor(div_, tmp, operands, NULL, 2, NULL, 0);

            switch (parser->relation_operator) {
                case T_GREATER:
                    rel->instruct = gt;
                    break;
                case T_LESS:
                    rel->instruct = lt;
                    break;
                case T_GREATER_EQUAL:
                    rel->instruct = gte;
                    break;
                case T_LESS_EQUAL:
                    rel->instruct = lte;
                    break;
                default:
                    exit(SYNTAX_ERROR);
            }

            generator_add_instruction(gen, rel);

            stack_push(parser->garbage_bin, op_one);
            stack_push(parser->garbage_bin, op_two);

            printf("8p ");
            stack_push(temps, data);
            stack_push(stack, stack_data(P_E, P_E));
            parser->relation_operator = 0;
            return 1;
        case 72: // E===E
            while (cnt < 5) {
                const TData *garbage = stack_pop(shelf);
                if (garbage == NULL) {
                    break;
                }
                stack_push(parser->garbage_bin, garbage);
                cnt++;
            }
            if (cnt != 5) break;
            op_one = stack_pop(temps);
            op_two = stack_pop(temps);
            htab_data->value_type = D_BOOL;

            defvar_order(tmp, htab_data, gen, parser);

            operands = malloc(sizeof(htab_data_t*) * 2);
            operands[1] = op_one->bucket; //reversed due to stack
            operands[0] = op_two->bucket;
            generator_add_instruction(gen, gen_instruction_constructor(eq, tmp, operands, NULL, 2, NULL, 0));

            stack_push(parser->garbage_bin, op_one);
            stack_push(parser->garbage_bin, op_two);

            printf("9p ");
            stack_push(temps, data);
            stack_push(stack, stack_data(P_E, P_E));
            parser->relation_operator = 0;
            return 1;
        case 73: // E!==E
            while (cnt < 5) {
                const TData *garbage = stack_pop(shelf);
                if (garbage == NULL) {
                    break;
                }
                stack_push(parser->garbage_bin, garbage);
                cnt++;
            }
            if (cnt != 5) break;
            op_one = stack_pop(temps);
            op_two = stack_pop(temps);
            htab_data->value_type = D_BOOL;

            defvar_order(tmp, htab_data, gen, parser);

            operands = malloc(sizeof(htab_data_t*) * 2);
            operands[1] = op_one->bucket; //reversed due to stack
            operands[0] = op_two->bucket;
            generator_add_instruction(gen, gen_instruction_constructor(neq, tmp, operands, NULL, 2, NULL, 0));

            stack_push(parser->garbage_bin, op_one);
            stack_push(parser->garbage_bin, op_two);

            printf("10p ");
            stack_push(temps, data);
            stack_push(stack, stack_data(P_E, P_E));
            parser->relation_operator = 0;
            return 1;
        default:
            break;
    }

    free(data);
    stack_dispose(shelf);
    return 0;
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

/// \brief Support function for storing and fetching tokens
/// \param token Token structure
/// \param keep_token indicates whether the token should be kept or not
/// \param return_back indicates whether the token should be returned back to the input
/// \param scanner Scanner structure
/// \param parser Parser structure
void get_next_token(Token **token, bool * restrict keep_token, bool * restrict return_back,
                    Scanner * restrict scanner, Parser * restrict parser) {
    if (*keep_token && *return_back) {
        Token *tmp = *token;
        (*token) = parser->tmp_token;
        parser->tmp_token = tmp;
    }
    else if (!(*keep_token) && *return_back) {
        Token *tmp = *token;
        (*token) = parser->tmp_token;
        parser->tmp_token = tmp;
        *return_back = false;
    }
    else if (!(*keep_token)){
        /* copy previous token to temporary token */
        parser->tmp_token->line = (*token)->line;
        parser->tmp_token->value = (*token)->value;
        parser->tmp_token->type = (*token)->type;

        /* get new token from scanner */
        (*token)->value.identifier = NULL;
        get_token((*token), scanner);
        if ((*token)->type == T_ERROR) exit(LEXICAL_ERROR);
    }
    *keep_token = false;
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
