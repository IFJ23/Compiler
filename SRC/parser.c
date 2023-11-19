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
#include "string.h"

#define LEXICAL_OK 0

#define TABLE_SIZE 10

// Precedence table (adjusted without | and $)
const unsigned int PREC_TABLE[17][17] = {
//           *     /     \     +     -     =    !=     <    <=     >    >=     (     )     ID    F     ,     ??
/*  *  */ { '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' , '_' , '_' , '>' },
/*  /  */ { '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' , '_' , '_' , '>' },
/*  \  */ { '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' , '_' , '_' , '>' },
/*  +  */ { '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' , '_' , '_' , '>' },
/*  -  */ { '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' , '_' , '_' , '>' },
/*  =  */ { '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' , '_' , '_' , '>' },
/*  != */ { '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' , '_' , '_' , '>' },
/*  <  */ { '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' , '_' , '_' , '>' },
/*  <= */ { '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' , '_' , '_' , '>' },
/*  >  */ { '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' , '_' , '_' , '>' },
/*  >= */ { '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' , '_' , '_' , '>' },
/*  (  */ { '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '=' , '<' , '_' , '<' , '_' },
/*  )  */ { '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '_' , '>' , '_' , '_' , '_' , '>' },
/*  ID */ { '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '_' , '>' , '_' , '_' , '>' , '>' },
/*  F  */ { '_' , '_' , '_' , '_' , '_' , '_' , '_' , '_' , '_' , '_' , '_' , '<' , '_' , '_' , '_' , '_' , '_' },
/*  ,  */ { '_' , '_' , '_' , '_' , '_' , '_' , '_' , '_' , '_' , '_' , '_' , '_' , '<' , '<' , '_' , '<' , '_' },
/*  ??  */ { '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '_' , '_' },
};

const unsigned int LL_TABLE[13][29] = {
        {1,2},                                                        // <prog>
        {[2] = 3, [28] = 4},                                                   // <scope>
        {[8] = 5, 0, 8, 6, 7, 9},                     // <statement>
        {[10] = 11, [17] = 10},                                                // <return_follow>
        {[9] = 13, [17] = 12},                                                 // <else>
        {[8] = 15, [15] = 14},                                                 // <else_n>
        {[8] = 17, [10] = 17, 17, 17, 17, 17, [17] = 16}, // <body>
        {[18] = 18, 19, 20, 21, 22, 23},            // <type>
        {[24] = 24, 25, 26, 27},                              // <value>
        {[28] = {28, 29}},                                                    // <print>
        {[5] = 31, 32, 33, [14] = 30},                              // <params>
        {[14] = 34, 0, 35},                                        // <params_n>
        {[1] = 36, 0, 37}                                           // <end>
};

/**
 * Function tests if symbols in parameters are valid according to rules.
 *
 * @param num Number of valid symbols in parameter.
 * @param op1 Symbol 1.
 * @param op2 Symbol 2.
 * @param op3 Symbol 3.
 * @return NOT_A_RULE if no rule is found or returns rule which is valid.
 */
static Prec_rules_enum test_rule(int num, Symbol_stack_item* op1, Symbol_stack_item* op2, Symbol_stack_item* op3)
{
    switch (num)
    {
        case 1:
            // rule E -> i
            if (op1->symbol == IDENTIFIER || op1->symbol == INT_NUMBER || op1->symbol == DOUBLE_NUMBER || op1->symbol == STRING)
                return OPERAND;

            return NOT_A_RULE;

        case 3:
            // rule E -> (E)
            if (op1->symbol == LEFT_BRACKET && op2->symbol == NON_TERM && op3->symbol == RIGHT_BRACKET)
                return LBR_NT_RBR;

            if (op1->symbol == NON_TERM && op3->symbol == NON_TERM)
            {
                switch (op2->symbol)
                {
                    // rule E -> E + E
                    case PLUS:
                        return NT_PLUS_NT;

                        // rule E -> E - E
                    case MINUS:
                        return NT_MINUS_NT;

                        // rule E -> E * E
                    case MUL:
                        return NT_MUL_NT;

                        // rule E -> E / E
                    case DIV:
                        return NT_DIV_NT;

                        // rule E -> E \ E
                    case IDIV:
                        return NT_IDIV_NT;

                        // rule E -> E = E
                    case EQ:
                        return NT_EQ_NT;

                        // rule E -> E <> E
                    case NEQ:
                        return NT_NEQ_NT;

                        // rule E -> E <= E
                    case LEQ:
                        return NT_LEQ_NT;

                        // rule E -> E < E
                    case LTN:
                        return NT_LTN_NT;

                        // rule E -> E >= E
                    case MEQ:
                        return NT_MEQ_NT;

                        // rule E -> E > E
                    case MTN:
                        return NT_MTN_NT;

                        // invalid operator
                    default:
                        return NOT_A_RULE;
                }
            }
            return NOT_A_RULE;
    }
    return NOT_A_RULE;
}

/**
 * Function checks semantics of operands according to rule.
 *
 * @param rule Pointer to table.
 * @param op1 Symbol 1.
 * @param op2 Symbol 2.
 * @param op3 Symbol 3.
 * @param final_type Sets data type which will be after executing rule.
 * @return Given exit code.
 */
static int check_semantics(Prec_rules_enum rule, Symbol_stack_item* op1, Symbol_stack_item* op2, Symbol_stack_item* op3, Data_type* final_type)
{
    bool retype_op1_to_double = false;
    bool retype_op3_to_double = false;
    bool retype_op1_to_integer = false;
    bool retype_op3_to_integer = false;

    if (rule == OPERAND)
    {
        if (op1->data_type == TYPE_UNDEFINED)
            return SEM_ERR_UNDEFINED_VAR;

        if (op1->data_type == TYPE_BOOL)
            return SEM_ERR_TYPE_COMPAT;
    }

    if (rule == LBR_NT_RBR)
    {
        if (op2->data_type == TYPE_UNDEFINED)
            return SEM_ERR_UNDEFINED_VAR;
    }

    if (rule != OPERAND && rule != LBR_NT_RBR)
    {
        if (op1->data_type == TYPE_UNDEFINED || op3->data_type == TYPE_UNDEFINED)
            return SEM_ERR_UNDEFINED_VAR;

        if (op1->data_type == TYPE_BOOL || op3->data_type == TYPE_BOOL)
            return SEM_ERR_TYPE_COMPAT;
    }

    switch (rule)
    {
        case OPERAND:
            *final_type = op1->data_type;
            break;

        case LBR_NT_RBR:
            *final_type = op2->data_type;
            break;

        case NT_PLUS_NT:
        case NT_MINUS_NT:
        case NT_MUL_NT:
            if (op1->data_type == TYPE_STRING && op3->data_type == TYPE_STRING && rule == NT_PLUS_NT)
            {
                *final_type = TYPE_STRING;
                break;
            }

            if (op1->data_type == TYPE_INT && op3->data_type == TYPE_INT)
            {
                *final_type = TYPE_INT;
                break;
            }

            if (op1->data_type == TYPE_STRING || op3->data_type == TYPE_STRING)
                return SEM_ERR_TYPE_COMPAT;

            *final_type = TYPE_DOUBLE;

            if (op1->data_type == TYPE_INT)
                retype_op1_to_double = true;

            if (op3->data_type == TYPE_INT)
                retype_op3_to_double = true;

            break;

        case NT_DIV_NT:
            *final_type = TYPE_DOUBLE;

            if (op1->data_type == TYPE_STRING || op3->data_type == TYPE_STRING)
                return SEM_ERR_TYPE_COMPAT;

            if (op1->data_type == TYPE_INT)
                retype_op1_to_double = true;

            if (op3->data_type == TYPE_INT)
                retype_op3_to_double = true;

            break;

        case NT_IDIV_NT:
            *final_type = TYPE_INT;

            if (op1->data_type == TYPE_STRING || op3->data_type == TYPE_STRING)
                return SEM_ERR_TYPE_COMPAT;

            if (op1->data_type == TYPE_DOUBLE)
                retype_op1_to_integer = true;

            if (op3->data_type == TYPE_DOUBLE)
                retype_op3_to_integer = true;

            break;

        case NT_EQ_NT:
        case NT_NEQ_NT:
        case NT_LEQ_NT:
        case NT_LTN_NT:
        case NT_MEQ_NT:
        case NT_MTN_NT:
            *final_type = TYPE_BOOL;

            if (op1->data_type == TYPE_INT && op3->data_type == TYPE_DOUBLE)
                retype_op1_to_double = true;

            else if (op1->data_type == TYPE_DOUBLE && op3->data_type == TYPE_INT)
                retype_op3_to_double = true;

            else if (op1->data_type != op3->data_type)
                return SEM_ERR_TYPE_COMPAT;

            break;

        default:
            break;
    }

    if (retype_op1_to_double)
    {
        GENERATE_CODE(generate_stack_op2_to_double);
    }

    if (retype_op3_to_double)
    {
        GENERATE_CODE(generate_stack_op1_to_double);
    }

    if (retype_op1_to_integer)
    {
        GENERATE_CODE(generate_stack_op2_to_integer);
    }

    if (retype_op3_to_integer)
    {
        GENERATE_CODE(generate_stack_op1_to_integer);
    }

    return SYNTAX_OK;
}

/**
 * Function reduces symbols after STOP symbol if rule for reducing is found.
 *
 * @param data Pointer to table.
 * @return Given exit code.
 */
static int reduce_by_rule(PData* data)
{
    (void) data;

    int result;

    Symbol_stack_item* op1 = NULL;
    Symbol_stack_item* op2 = NULL;
    Symbol_stack_item* op3 = NULL;
    Data_type final_type;
    Prec_rules_enum rule_for_code_gen;
    bool found = false;

    int count = num_of_symbols_after_stop(&found);


    if (count == 1 && found)
    {
        op1 = stack.top;
        rule_for_code_gen = test_rule(count, op1, NULL, NULL);
    }
    else if (count == 3 && found)
    {
        op1 = stack.top->next->next;
        op2 = stack.top->next;
        op3 = stack.top;
        rule_for_code_gen = test_rule(count, op1, op2, op3);
    }
    else
        return SYNTAX_ERR;

    if (rule_for_code_gen == NOT_A_RULE)
    {
        return SYNTAX_ERR;
    }
    else
    {
        if ((result = check_semantics(rule_for_code_gen, op1, op2, op3, &final_type)))
            return result;

        if (rule_for_code_gen == NT_PLUS_NT && final_type == TYPE_STRING)
        {
            GENERATE_CODE(generate_concat_stack_strings);
        }
        else GENERATE_CODE(generate_stack_operation, rule_for_code_gen);

        symbol_stack_pop_count(&stack, count + 1);
        symbol_stack_push(&stack, NON_TERM, final_type);
    }

    return SYNTAX_OK;
}

int expression(PData* data)
{
    int result = SYNTAX_ERR;

    symbol_stack_init(&stack);

    if (!symbol_stack_push(&stack, DOLLAR, TYPE_UNDEFINED))
        FREE_RESOURCES_RETURN(ERROR_INTERNAL);

    Symbol_stack_item* top_stack_terminal;
    Prec_table_symbol_enum actual_symbol;

    bool success = false;

    do
    {
        actual_symbol = get_symbol_from_token(&data->token);
        top_stack_terminal = symbol_stack_top_terminal(&stack);

        if (top_stack_terminal == NULL)
            FREE_RESOURCES_RETURN(ERROR_INTERNAL);

        switch (prec_table[get_prec_table_index(top_stack_terminal->symbol)][get_prec_table_index(actual_symbol)])
        {
            case S:
                if (!symbol_stack_insert_after_top_terminal(&stack, STOP, TYPE_UNDEFINED))
                    FREE_RESOURCES_RETURN(ERROR_INTERNAL);

                if(!symbol_stack_push(&stack, actual_symbol, get_data_type(&data->token, data)))
                    FREE_RESOURCES_RETURN(ERROR_INTERNAL);

                if (actual_symbol == IDENTIFIER || actual_symbol == INT_NUMBER || actual_symbol == DOUBLE_NUMBER || actual_symbol == STRING)
                {
                    GENERATE_CODE(generate_push, data->token);
                }

                if ((result = get_next_token(&data->token)))
                    FREE_RESOURCES_RETURN(result);
                break;

            case E:
                symbol_stack_push(&stack, actual_symbol, get_data_type(&data->token, data));

                if ((result = get_next_token(&data->token)))
                    FREE_RESOURCES_RETURN(result);
                break;

            case R:
                if ((result = reduce_by_rule(data)))
                    FREE_RESOURCES_RETURN(result);
                break;

            case N:
                if (actual_symbol == DOLLAR && top_stack_terminal->symbol == DOLLAR)
                    success = true;
                else
                    FREE_RESOURCES_RETURN(SYNTAX_ERR);
                break;
        }
    } while (!success);

    Symbol_stack_item* final_non_terminal = symbol_stack_top(&stack);
    if (final_non_terminal == NULL)
        FREE_RESOURCES_RETURN(ERROR_INTERNAL);
    if (final_non_terminal->symbol != NON_TERM)
        FREE_RESOURCES_RETURN(SYNTAX_ERR);

    if (data->lhs_id != NULL)
    {
        char *frame = "LF";
        if (data->lhs_id->global) frame = "GF";

        switch (data->lhs_id->type)
        {
            case TYPE_INT:
                if (final_non_terminal->data_type == TYPE_STRING)
                    FREE_RESOURCES_RETURN(SEM_ERR_TYPE_COMPAT);

                GENERATE_CODE(generate_save_expression_result, data->lhs_id->identifier, final_non_terminal->data_type, TYPE_INT, frame);
                break;

            case TYPE_DOUBLE:
                if (final_non_terminal->data_type == TYPE_STRING)
                    FREE_RESOURCES_RETURN(SEM_ERR_TYPE_COMPAT);

                GENERATE_CODE(generate_save_expression_result, data->lhs_id->identifier, final_non_terminal->data_type, TYPE_DOUBLE, frame);
                break;

            case TYPE_STRING:
                if (final_non_terminal->data_type != TYPE_STRING)
                    FREE_RESOURCES_RETURN(SEM_ERR_TYPE_COMPAT);

                GENERATE_CODE(generate_save_expression_result, data->lhs_id->identifier, TYPE_STRING, TYPE_STRING, frame);
                break;

            case TYPE_UNDEFINED:
                GENERATE_CODE(generate_save_expression_result, data->lhs_id->identifier, final_non_terminal->data_type, TYPE_UNDEFINED, frame);
                break;

            case TYPE_BOOL:
                if (final_non_terminal->data_type != TYPE_BOOL)
                    FREE_RESOURCES_RETURN(SEM_ERR_TYPE_COMPAT);

                GENERATE_CODE(generate_save_expression_result, data->lhs_id->identifier, final_non_terminal->data_type, TYPE_BOOL, frame);
                break;

            default:
                break;
        }
    }

    FREE_RESOURCES_RETURN(SYNTAX_OK);
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
