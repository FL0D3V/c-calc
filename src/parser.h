// Tries to parse a given lexed tokens-list and generates an AST for checking the order of operations in the math equation.

#ifndef _PARSER_H_
#define _PARSER_H_

#include <math.h>

#include "global.h"
#include "lexer.h"


typedef enum {
    NT_CONSTANT,
    NT_BINOP,
    NT_FUNCTION,
    NT_BRACKET,

    NT_COUNT
} e_node_type;

static_assert(NT_COUNT == 4, "Amount of node-types have changed");
const char* nodeTypeNames[NT_COUNT] = {
  [NT_CONSTANT] = "constant",
  [NT_BINOP] = "operator",
  [NT_FUNCTION] = "function",
  [NT_BRACKET] = "bracket"
};


typedef enum {
  NO_ADD,
  NO_SUB,
  NO_MUL,
  NO_DIV,
  NO_POW,

  NO_COUNT
} e_node_binop_type;

static_assert(NO_COUNT == 5, "Amount of binop-node-types have changed");
const char* nodeBinopTypeNames[NO_COUNT] = {
  [NO_ADD] = "add",
  [NO_SUB] = "substract",
  [NO_MUL] = "multiply",
  [NO_DIV] = "divide",
  [NO_POW] = "pow"
};


typedef enum {
  NF_SQRT,
  NF_SIN,
  NF_COS,
  NF_TAN,
  NF_LN,

  NF_COUNT
} e_node_func_type;

static_assert(NF_COUNT == 5, "Amount of function-node-types have changed");
const char* nodeFunctionTypeNames[NF_COUNT] = {
  [NF_SQRT] = "sqrt",
  [NF_SIN] = "sin",
  [NF_COS] = "cos",
  [NF_TAN] = "tan",
  [NF_LN] = "ln"
};


typedef enum {
  NB_PAREN,

  NB_COUNT
} e_node_bracket_type;

static_assert(NB_COUNT == 1, "Amount of bracket-node-types have changed");
const char* nodeBracketTypeNames[NB_COUNT] = {
  [NB_PAREN] = "parenthesis",
};



typedef struct node node_t;

typedef struct {
    e_node_binop_type type;
    node_t* lhs;
    node_t* rhs;
} node_binop_t;

typedef struct {
  e_node_func_type type;
  node_t* arg;
} node_function_t;

typedef struct {
  e_node_bracket_type type;
  node_t* arg;
} node_bracket_t;

typedef union {
  double constant;
  node_binop_t binop;
  node_function_t func;
  node_bracket_t bracket;
} u_node_as;

struct node {
    e_node_type type;
    // TODO: Rethink if needed!
    size_t cursor;
    u_node_as as;
};


// TODO: Implement arena allocator!
node_t* base_node(size_t cursor, e_node_type type)
{
  node_t* node = (node_t*) malloc(sizeof(node_t));
  node->type = type;
  node->cursor = cursor;
  return node;
}

node_t* node_constant(size_t cursor, double constant)
{
  node_t* node = base_node(cursor, NT_CONSTANT);
  node->as.constant = constant;
  return node;
}

node_t* node_binop(size_t cursor, e_node_binop_type type, node_t* lhs, node_t* rhs)
{
  node_t* node = base_node(cursor, NT_BINOP);
  node->as.binop.type = type;
  node->as.binop.lhs = lhs;
  node->as.binop.rhs = rhs;
  return node;
}

node_t* node_func(size_t cursor, e_node_func_type type, node_t* arg)
{
  node_t* node = base_node(cursor, NT_FUNCTION);
  node->as.func.type = type;
  node->as.func.arg = arg;
  return node;
}

node_t* node_bracket(size_t cursor, e_node_bracket_type type, node_t* arg)
{
  node_t* node = base_node(cursor, NT_BRACKET);
  node->as.bracket.type = type;
  node->as.bracket.arg = arg;
  return node;
}



void node_free(node_t* node)
{
  if (!node)
    return;

  switch (node->type) {
    case NT_CONSTANT:
      free(node);
      break;
    case NT_BINOP:
      node_free(node->as.binop.lhs);
      node_free(node->as.binop.rhs);
      break;
    case NT_FUNCTION:
      node_free(node->as.func.arg);
      break;
    case NT_BRACKET:
      node_free(node->as.bracket.arg);
      break;
    case NT_COUNT:
    default:
      UNREACHABLE("Invalid node-type!");
  }
}


// TODO: Print indented!
void print_node(node_t* node)
{
  ASSERT_NULL(node);

  switch (node->type) {
    case NT_CONSTANT:
      // TODO: Does not print correctly after around 5 decimal digits currently!
      printf("%.05lf", node->as.constant);
      break;
    case NT_BINOP:
      if (node->as.binop.type >= NO_COUNT)
        UNREACHABLE("Invalid node-binop-type!");

      printf("%s(", nodeBinopTypeNames[node->as.binop.type]);
      print_node(node->as.binop.lhs);
      printf(", ");
      print_node(node->as.binop.rhs);
      printf(")");
      break;
    case NT_FUNCTION:
      if (node->as.func.type >= NF_COUNT)
        UNREACHABLE("Invalid node-func-type!");

      printf("%s(", nodeFunctionTypeNames[node->as.func.type]);
      print_node(node->as.func.arg);
      printf(")");
      break;
    case NT_BRACKET:
      if (node->as.bracket.type >= NB_COUNT)
        UNREACHABLE("Invalid node-bracket-type!");

      printf("%s(", nodeBracketTypeNames[node->as.bracket.type]);
      print_node(node->as.bracket.arg);
      printf(")");
      break;
    case NT_COUNT:
    default:
      UNREACHABLE("Invalid node-type!");
  }
}

#define print_node_ln(node) (print_node(node), printf("\n"))


node_t* eval(node_t* expr)
{
  ASSERT_NULL(expr);

  switch (expr->type) {
    case NT_CONSTANT:
      return expr;
    case NT_BINOP:
      switch (expr->as.binop.type) {
        case NO_ADD: {
          node_t* lhs = eval(expr->as.binop.lhs);
          if (!lhs) return NULL;
          node_t* rhs = eval(expr->as.binop.rhs);
          if (!rhs) return NULL;
          return node_constant(expr->cursor, lhs->as.constant + rhs->as.constant);
        }
        case NO_SUB: {
          node_t* lhs = eval(expr->as.binop.lhs);
          if (!lhs) return NULL;
          node_t* rhs = eval(expr->as.binop.rhs);
          if (!rhs) return NULL;
          return node_constant(expr->cursor, lhs->as.constant - rhs->as.constant);
        }
        case NO_MUL: {
          node_t* lhs = eval(expr->as.binop.lhs);
          if (!lhs) return NULL;
          node_t* rhs = eval(expr->as.binop.rhs);
          if (!rhs) return NULL;
          return node_constant(expr->cursor, lhs->as.constant * rhs->as.constant);
        }
        case NO_DIV: {
          node_t* lhs = eval(expr->as.binop.lhs);
          if (!lhs) return NULL;
          node_t* rhs = eval(expr->as.binop.rhs);
          if (!rhs) return NULL;
          // TODO: Rethink if DIVIDE BY ZERO should be checked!
          return node_constant(expr->cursor, lhs->as.constant / rhs->as.constant);
        }
        case NO_POW: {
          node_t* lhs = eval(expr->as.binop.lhs);
          if (!lhs) return NULL;
          node_t* rhs = eval(expr->as.binop.rhs);
          if (!rhs) return NULL;
          return node_constant(expr->cursor, pow(lhs->as.constant, rhs->as.constant));
        }
        case NO_COUNT:
        default:
          UNREACHABLE("Invalid binop-node-type!");
      }
      break;
    case NT_FUNCTION:
      switch (expr->as.func.type) {
        case NF_SQRT: {
          node_t* func = eval(expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(expr->cursor, sqrt(func->as.constant));
        }
        case NF_SIN: {
          node_t* func = eval(expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(expr->cursor, sin(func->as.constant));
        }
        case NF_COS: {
          node_t* func = eval(expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(expr->cursor, cos(func->as.constant));
        }
        case NF_TAN: {
          node_t* func = eval(expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(expr->cursor, tan(func->as.constant));
        }
        case NF_LN: {
          node_t* func = eval(expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(expr->cursor, log(func->as.constant));
        }
        case NO_COUNT:
        default:
          UNREACHABLE("Invalid function-node-type!");
      }
      break;
    case NT_BRACKET:
      switch(expr->as.bracket.type) {
        case NB_PAREN: {
          node_t* bracket = eval(expr->as.bracket.arg);
          if (!bracket) return NULL;
          return node_constant(expr->cursor, bracket->as.constant);
        }
        case NB_COUNT:
        default:
          UNREACHABLE("Invalid bracket-node-type!");
      }
      break;
    case NT_COUNT:
    default:
      UNREACHABLE("Invalid node-type!");
  }
}



node_t* parse_lexer(lexer_t* lexer)
{
  ASSERT_NULL(lexer);

  if (lexer->isError)
    return NULL;

  // TODO:
  // - Memory allocation
  // - Operation order checking (operators +,-,*,/,^ and brackets '(', ')')
  // - - Order: Brackets -> Exponents -> Multiplication -> Division -> Addition -> Substraction
  // - A stack for checking nested expressions in brackets. Also if the brackets are used correctly.
  // - - 2 Open '(' should have 2 Closing ')' after.
  // - After a constant MUST come an operator, function, or bracket

  //for (size_t i = 0; i < lexer->count; ++i)
  //{
  //  break;
  //}

  // TODO: Return root node!
  return NULL;  
}

#endif // _PARSER_H_
