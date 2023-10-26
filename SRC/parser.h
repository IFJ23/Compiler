#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "symtable.h"
#include "stack.h"
#include "generator.h"

extern const unsigned int LL_TABLE[8][33];

extern const unsigned int PREC_TABLE[14][14];

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

typedef enum {
    P_MUL __attribute__((unused)),
    P_DIV __attribute__((unused)),
    P_ADD __attribute__((unused)),
    P_SUB __attribute__((unused)),
    P_CONC __attribute__((unused)),
    P_I,
    P_LEFT_BRACKET,
    P_RIGHT_BRACKET,
    P_R,
    P_TYPE_EQ __attribute__((unused)),
    P_TYPE_NEQ __attribute__((unused)),
    P_END,
    P_FN,
    P_COMMA,
    P_OPEN,
    P_CLOSE,
    P_EQUAL,
    P_E
}PrecSyms;

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