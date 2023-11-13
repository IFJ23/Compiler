#ifndef _AST_H
#define _AST_H 1

#define AST_DEBUG 0

#include <stdint.h>
#include "symtable.h"

typedef STDataType ASTDataType;

typedef enum ast_node_action_type {
    // ARITHMETIC group
#define AST_ARITHMETIC 0
    AST_ADD = AST_ARITHMETIC,
    AST_SUBTRACT,
    AST_MULTIPLY,
    AST_DIVIDE,
    AST_AR_NEGATE,
    // LOGIC group
#define AST_LOGIC 100
    AST_LOG_NOT = AST_LOGIC,
    AST_LOG_AND,
    AST_LOG_OR,
    AST_LOG_EQ,
    AST_LOG_NEQ,
    AST_LOG_LT,
    AST_LOG_GT,
    AST_LOG_LTE,
    AST_LOG_GTE,
    // CONTROL group
#define AST_CONTROL 200
    AST_ASSIGN = AST_CONTROL,
    AST_DEFINE,
    AST_FUNC_CALL,
    // VALUE group
#define AST_VALUE 300
    AST_LIST = AST_VALUE,
    AST_ID,
    AST_CONST_INT,
    AST_CONST_DOUBLE,
    AST_CONST_STRING,
} ASTNodeType;

struct ast_node;

typedef union ast_node_data {
    STSymbol *symbolTableItemPtr;
    struct ast_node *astPtr;
    int64_t intConstantValue;
    double floatConstantValue;
    const char *stringConstantValue;
    bool boolConstantValue;
} ASTNodeData;

typedef struct ast_node {
    struct ast_node *parent;
    ASTNodeType actionType;
    ASTDataType inheritedDataType;
    struct ast_node *left;
    struct ast_node *right;

    bool hasInnerFuncCalls;
    unsigned dataCount;
    unsigned dataPointerIndex;
    ASTNodeData data[];
} ASTNode;

typedef enum ast_error {
    AST_NO_ERROR = 0,
    AST_ERROR_SYMBOL_NOT_ASSIGNED,
    AST_ERROR_BINARY_OP_CHILDREN_NOT_ASSIGNED,
    AST_ERROR_UNARY_OP_CHILD_NOT_ASSIGNED,
    AST_ERROR_BINARY_OP_TYPES_MISMATCH,
    AST_ERROR_INVALID_CHILDREN_TYPE_FOR_OP,
    AST_ERROR_LIST_FULL,
    AST_ERROR_INVALID_TARGET,
    AST_ERROR_INVALID_ARGUMENT,
    AST_ERROR_INTERNAL
} ASTError;

// Holds the current error state. The "no error" state is guaranteed to be a zero,
// so an error check may be performed using `if (cf_error)`.
extern ASTError ast_error;

// Allocates and returns a new empty AST node.
// Does NOT run type inference.
ASTNode *ast_node(ASTNodeType nodeType);

// Clears memory allocated by ASTNode
void clean_ast(ASTNode *node);

// Checks whether an AST has no effect.
bool is_ast_empty(ASTNode *ast);

// Allocates and returns a new empty AST node with the specified amount of data space.
// Does NOT run type inference.
ASTNode *ast_node_data(ASTNodeType nodeType, unsigned dataCount);

// Allocates and returns a new AST node with space for one data container and assigns the data.
// This function would typically be used with node types ID or CONST_*.
// Runs type inference.
ASTNode *ast_leaf_single_data(ASTNodeType nodeType, ASTNodeData data);

// Allocates and returns a new AST node with the AST_LIST type and the specified amount of inner AST nodes.
// Does NOT run type inference.
ASTNode *ast_node_list(unsigned dataCount);

// Allocates and returns a new AST (leaf) node with the AST_ID type and assigns the specified symbol pointer
// to its data.
// Runs type inference.
ASTNode *ast_leaf_id(STSymbol *idSymbolPtr);

// Allocates and returns a new AST (leaf) node with the AST_ID type and sets it to the CF_BLACK_HOLE type with no
// assigned symbol.
ASTNode *ast_leaf_black_hole();

// Allocates and returns a new AST node with the AST_FUNC_CALL type and assigns
// the specified AST_LIST node as its parameters list (the RIGHT child).
// The function actually allocates two nodes: one for the function call itself, which is returned,
// and one AST_ID node that holds the symbol table pointer to the identifier of the function. This node is assigned
// to the returned node LEFT child.
// Runs type inference for both the inner AST_ID node and the return AST_FUNC_CALL node.
// The inferred type WILL be equal in both nodes and MAY end up being CF_NIL for functions with no return value
// or CF_MULTIPLE if the function returns more values.
ASTNode *ast_node_func_call(STSymbol *funcIdSymbolPtr, ASTNode *paramListNode);

// Allocates and returns a new AST (leaf) node with the AST_CONST_INT type and assigns the specified integer constant
// to its data.
// Runs type inference.
ASTNode *ast_leaf_consti(int i);

// Allocates and returns a new AST (leaf) node with the AST_CONST_FLOAT type and assigns the specified float constant
// to its data.
// Runs type inference.
ASTNode *ast_leaf_constf(double f);

// Allocates and returns a new AST (leaf) node with the AST_CONST_STRING type and assigns the specified string constant
// to its data.
// Runs type inference.
ASTNode *ast_leaf_consts(const char *s);

// Allocates and returns a new AST (leaf) node with the AST_CONST_BOOL type and assigns the specified boolean constant
// to its data.
// Runs type inference.
ASTNode *ast_leaf_constb(bool b);

// Finds the parent AST_LIST node for the specified node, which is a part of the returned AST_LIST data hierarchy.
// If innerNode is not a descendant of an AST_LIST, NULL is returned.
ASTNode *ast_get_list_root(ASTNode *innerNode);

// Sets the specified node as the specified AST_LIST's data on the specified index.
void ast_add_to_list(ASTNode *astList, ASTNode *node, unsigned dataIndex);

// Sets the specified node as the specified AST_LIST's data on the first position that hasn't been yet set.
// Returns the set position or INT_MAX when all the data has been pushed already (and sets an error in this case).
unsigned ast_push_to_list(ASTNode *astList, ASTNode *node);

/* Infers the data type for the specified leaf (ID/CONST_*) node and saves in in the node, if the current node type is CF_UNKNOWN.
 * If node is an AST_ID node, looks for the type in the associated symbol. Throws an error, returns false
 * and sets the node type to CF_UNKNOWN_UNINFERRABLE if no symbol is associated with the node (in its data).
 *
 * If the associated symbol is a function symbol, one of the following may occur:
 *  - Function has no return values => CF_NIL is set as the node's type.
 *  - Function has one return value => its type is set as the node's type.
 *  - Function has more return values => CF_MULTIPLE is set as the node's type.
 *
 * Variable symbols always have one type which is stored in the symbol; the type of a constant is apparent.
 * If node in not and ID or a CONST_* node, CF_UNKNOWN is set as the node's type and false is returned.
 */
bool ast_infer_leaf_type(ASTNode *node);

/* Infers the data type for the specified node based on its type and its children, if the current node type is CF_UNKNOWN.
 * If the node has children and their type is set to CF_UNKNOWN, type inference is run on them as well, recursively.
 * When this function returns false, the node type is set to CF_UNKNOWN_UNINFERRABLE.
 *
 * For binary nodes:
 *  - Both LEFT and RIGHT children must not be null.
 *  - Both LEFT and RIGHT children must have had their type inferred successfully.
 *  - LEFT and RIGHT children's types must be the same.
 *  - The inferred type is equal to the children's type.
 * For unary nodes:
 *  - LEFT (unary) child must not be null.
 *  - LEFT (unary) child must have had its type inferred successfully.
 *  - The inferred type is equal to the left child's type.
 * For logical nodes:
 *  - All significant children must have their inferred type equal to CF_BOOL.
 * For arithmetic nodes:
 *  - All significant children must have their inferred type equal either to CF_INT or to CF_FLOAT.
 * For comparison nodes:
 *  - The inferred type is set to CF_BOOL if types of significant children match.
 * AST_ASSIGN or AST_DEFINE node's type is set to CF_NIL and true is returned.
 * AST_FUNC_CALL node's type is set to its AST_ID child's type.
 * For AST_LIST nodes:
 *  - If the list has zero-length data, CF_NIL is set as its type.
 *  - If the list has one-length data, the node's type is inferred from the type of this data AST and true is returned.
 *    If this data AST is NULL or its type cannot be inferred, false is returned.
 *  - If the list has more data, CF_MULTIPLE is set as its type and true is returned.
 * For ID and CONST_* nodes, ast_infer_leaf_type() is called.
 */
bool ast_infer_node_type(ASTNode *node);

// Turns strict inference on or off. When strict inference is on, CF_UNKNOWN inference result is considered erroneous.
// This is used in the second pass of semantic checks, when all function definitions are available.
void ast_set_strict_inference_state(bool state);

ASTDataType ast_data_type_for_node_type(ASTNodeType nodeType);

#endif // _AST_H
