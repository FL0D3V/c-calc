// Tries to parse a given lexed tokens-list and generates an AST for checking the order of operations in the math equation.

#ifndef _PARSER_H_
#define _PARSER_H_

#include "global.h"
#include "lexer.h"


typedef enum {
    NT_NUMBER,

    NT_ADD,
    NT_SUB,
    NT_MUL,
    NT_DIV,
    NT_POW,

    NT_SQRT,
    NT_SIN,
    NT_COS,
    NT_TAN,
    NT_LN,

    NT_COUNT,
} e_node_type;

static_assert(NT_COUNT == 11, "Amount of nodes-types have changed");
const char *nk_names[NT_COUNT] = {
    [NT_NUMBER] = "number",
    
    [NT_ADD] = "add",
    [NT_SUB] = "substract",
    [NT_MUL] = "multiply",
    [NT_DIV] = "divide",
    [NT_POW] = "power",

    [NT_SQRT] = "sqrt",
    [NT_SIN] = "sin",
    [NT_COS] = "cos",
    [NT_TAN] = "tan",
    [NT_LN] = "ln",
};


typedef struct node node_t;

typedef struct {
    node_t* lhs;
    node_t* rhs;
} node_binop_t;

// TODO: Rethink!
typedef struct {
  node_t* constant;
} node_function_t;

typedef union {
  double number;
  node_binop_t binop;
  node_function_t func;
} u_node_as;

struct node {
    e_node_type type;
    size_t cursor;
    u_node_as as;
};


void parse_lexer(lexer_t* lexer)
{
  ASSERT_NULL(lexer);

  for (size_t i = 0; i < lexer->count; ++i)
  {
    UNREACHABLE("Not implemented!");
  }
}

#endif // _PARSER_H_
