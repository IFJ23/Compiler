// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "symtable.h"
#include "stack.h"
#include "generator.h"

extern const unsigned int LL_TABLE[13][29];

extern const unsigned int PREC_TABLE[17][17];

#define TEMP_VAR_PREFIX "$!!$tmp" // prefix for temporary variables
#define TEMP_LENGTH 8 // length of the prefix
#define EPS 14 // eps has column index 14 in ll table
#define GLOBTAB_SIZE 4003 // num of buckets to allocate for global symtab
#define LOCALTAB_SIZE 1103 // num of buckets to allocate for local symtabs

typedef enum {
    N_PROG,
    N_ST_LIST,
    N_ST,
    N_SMALL_ST,
    N_PARAM_DEF,
    N_PARAM_NAME,
    N_PARAM_LIST,
    N_BODY,
    N_EXPR
} NonTerm;

typedef enum {
    T_TERM,
    T_NONTERM,
    T_KW,
} TermType;

typedef enum
{
    S,    /// < SHIFT
    E,    /// = EQUAL
    R,    /// > REDUCE
    N     /// # ERROR
} Prec_table_sign_enum;

typedef enum
{
    I_NOT,             /// 0 ! (logical NOT)
    I_MUL_DIV,         /// 1 * /
    I_PLUS_MINUS,       /// 2 + -
    I_EQ_NE_REL,       /// 3 == != < > <= >= (equality and relational operators)
    I_COALESCE,        /// 4 ?? (null coalescing operator)
    I_LEFT_BRACKET,    /// 5 (
    I_RIGHT_BRACKET,   /// 6 )
    I_DATA            /// 7 i
} Prec_table_index_enum;

typedef struct parser_t {
    bool in_param_def;
    bool empty_expr;
    bool allow_expr_empty;
    bool expect_ret;
    bool if_eval;
    bool while_eval;
    bool only_defvar;
    char relation_operator;
    int while_count;
    int tmp_counter;
    int bracket_counter;
    char **builtins;
    Token *tmp_token;
    htab_t *glob_tab;
    htab_t *temporary_tab;
    Stack *local_tabs;
    Stack *garbage_bin;
    htab_data_t *in_func;
    htab_data_t *in_assign;
    htab_data_t *val_returned;
    struct Instruction *in_while;
    struct Instruction *in_fn;
} Parser;

#endif // PARSER_H