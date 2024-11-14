// Tries to parse a given lexed tokens-list and generates an AST for checking the order of operations in the math equation.

#ifndef _PARSER_H_
#define _PARSER_H_

#include <math.h>

#include "global.h"
#include "lexer.h"
#include "darray.h"


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
  NF_EXP,

  NF_SIN,
  NF_ASIN,
  NF_SINH,
  
  NF_COS,
  NF_ACOS,
  NF_COSH,
  
  NF_TAN,
  NF_ATAN,
  NF_TANH,
  
  NF_LN,
  NF_LOG10,

  NF_COUNT
} e_node_func_type;

static_assert(NF_COUNT == 13, "Amount of function-node-types have changed");
const char* nodeFunctionTypeNames[NF_COUNT] = {
  [NF_SQRT]  = "sqrt",
  [NF_EXP]   = "exp",

  [NF_SIN]   = "sin",
  [NF_ASIN]  = "asin",
  [NF_SINH]  = "sinh",

  [NF_COS]   = "cos",
  [NF_ACOS]  = "acos",
  [NF_COSH]  = "cosh",
  
  [NF_TAN]   = "tan",
  [NF_ATAN]  = "atan",
  [NF_TANH]  = "tanh",
  
  [NF_LN]    = "ln",
  [NF_LOG10] = "log10"
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



// TODO: Check why there are memory errors when this number is f.e. 2..20..!
#define NODE_REGION_DEFAULT_CAPACITY 128

typedef struct node_region node_region_t;

struct node_region {
  node_region_t* next;
  size_t capacity;
  size_t count;
  // The region will get allocated with the size of the region + the size of a node * capacity.
  // So this is just the starting pointer to the additional memory allocated with the size of capacity.
  node_t data[];
};

typedef struct {
  node_region_t* begin;
} node_arena_t;

static node_region_t* new_node_region(size_t capacity)
{
  // Allocates the size of a region + the size of a node * capacity.
  // Thats why the node array is at the end because it just stores the starting
  // pointer to the extra space (size of node * capacity).
  size_t size_bytes = sizeof(node_region_t) + sizeof(node_t) * capacity;
  node_region_t* region = (node_region_t*) malloc(size_bytes);
  assert(region && "Not enough memory!");
  region->next = NULL;
  region->count = 0;
  region->capacity = capacity;
  return region;
}

static void free_node_region(node_region_t* region)
{
  free(region);
}

node_t* node_arena_alloc(node_arena_t* arena)
{
  ASSERT_NULL(arena);

  node_region_t* region = NULL;
  
  if (!arena->begin) {
    arena->begin = new_node_region(NODE_REGION_DEFAULT_CAPACITY);
    region = arena->begin;
  } else {
    node_region_t* tmp = arena->begin;
  
    // TODO: Fix memory bug!
    while (tmp->count + 1 < tmp->capacity && tmp->next) {
      tmp = tmp->next;
    }
    
    ASSERT_NULL(tmp);

    if (tmp->count + 1 < tmp->capacity) {
      region = tmp;
    } else {
      tmp->next = new_node_region(NODE_REGION_DEFAULT_CAPACITY);
      region = tmp->next;
    }
  }
  
  ASSERT_NULL(region);
  return &region->data[region->count++];
}

void node_arena_free(node_arena_t* arena)
{
  ASSERT_NULL(arena);
  node_region_t* region = arena->begin;
  while (region) {
    node_region_t* tmp = region;
    region = region->next;
    free_node_region(tmp);
  }
  arena->begin = NULL;
}



node_t* base_node(node_arena_t* arena, size_t cursor, e_node_type type)
{
  node_t* newNode = node_arena_alloc(arena);
  newNode->type = type;
  newNode->cursor = cursor;
  return newNode;
}

node_t* node_constant(node_arena_t* arena, size_t cursor, double constant)
{
  node_t* node = base_node(arena, cursor, NT_CONSTANT);
  node->as.constant = constant;
  return node;
}

node_t* node_binop(node_arena_t* arena, size_t cursor, e_node_binop_type type, node_t* lhs, node_t* rhs)
{
  node_t* node = base_node(arena, cursor, NT_BINOP);
  node->as.binop.type = type;
  node->as.binop.lhs = lhs;
  node->as.binop.rhs = rhs;
  return node;
}

node_t* node_func(node_arena_t* arena, size_t cursor, e_node_func_type type, node_t* arg)
{
  node_t* node = base_node(arena, cursor, NT_FUNCTION);
  node->as.func.type = type;
  node->as.func.arg = arg;
  return node;
}

node_t* node_bracket(node_arena_t* arena, size_t cursor, e_node_bracket_type type, node_t* arg)
{
  node_t* node = base_node(arena, cursor, NT_BRACKET);
  node->as.bracket.type = type;
  node->as.bracket.arg = arg;
  return node;
}



#define _PRINT_DEPTH_SPACES(indented, deph) \
    do {                                    \
      if (indented)                         \
        for (size_t i = 0; i < (deph); ++i) \
          printf(" ");                      \
    } while(0)

void print_node_ex(node_t* node, bool indented, size_t deph)
{
  ASSERT_NULL(node);

  switch (node->type) {
    case NT_CONSTANT:
      _PRINT_DEPTH_SPACES(indented, deph);
      printf(DOUBLE_PRINT_FORMAT, node->as.constant);
      break;
    case NT_BINOP:
      if (node->as.binop.type >= NO_COUNT)
        UNREACHABLE("Invalid binop-node-type!");
      
      _PRINT_DEPTH_SPACES(indented, deph);
      printf("%s(", nodeBinopTypeNames[node->as.binop.type]);
      if (indented) printf("\n");
      print_node_ex(node->as.binop.lhs, indented, deph + 1);
      printf(",%s", indented ? "\n" : " ");
      print_node_ex(node->as.binop.rhs, indented, deph + 1);
      if (indented) printf("\n");
      _PRINT_DEPTH_SPACES(indented, deph);
      printf(")");
      break;
    case NT_FUNCTION:
      if (node->as.func.type >= NF_COUNT)
        UNREACHABLE("Invalid func-node-type!");
      
      _PRINT_DEPTH_SPACES(indented, deph);
      printf("%s(", nodeFunctionTypeNames[node->as.func.type]);
      if (indented) printf("\n");
      print_node_ex(node->as.func.arg, indented, deph + 1);
      if (indented) printf("\n");
      _PRINT_DEPTH_SPACES(indented, deph);
      printf(")");
      break;
    case NT_BRACKET:
      if (node->as.bracket.type >= NB_COUNT)
        UNREACHABLE("Invalid bracket-node-type!");

      _PRINT_DEPTH_SPACES(indented, deph);
      printf("%s(", nodeBracketTypeNames[node->as.bracket.type]);
      if (indented) printf("\n");
      print_node_ex(node->as.bracket.arg, indented, deph + 1);
      if (indented) printf("\n");
      _PRINT_DEPTH_SPACES(indented, deph);
      printf(")");
      break;
    case NT_COUNT:
    default:
      UNREACHABLE("Invalid node-type!");
  }
}

#define print_node(node, indented) (printf("Printing parsed AST:\n"), print_node_ex(node, indented, 0), printf("\n"))


node_t* eval(node_arena_t* arena, node_t* expr)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(expr);

  switch (expr->type) {
    case NT_CONSTANT:
      return expr;
    case NT_BINOP:
      switch (expr->as.binop.type) {
        case NO_ADD: {
          node_t* lhs = eval(arena, expr->as.binop.lhs);
          if (!lhs) return NULL;
          node_t* rhs = eval(arena, expr->as.binop.rhs);
          if (!rhs) return NULL;
          return node_constant(arena, expr->cursor, lhs->as.constant + rhs->as.constant);
        }
        case NO_SUB: {
          node_t* lhs = eval(arena, expr->as.binop.lhs);
          if (!lhs) return NULL;
          node_t* rhs = eval(arena, expr->as.binop.rhs);
          if (!rhs) return NULL;
          return node_constant(arena, expr->cursor, lhs->as.constant - rhs->as.constant);
        }
        case NO_MUL: {
          node_t* lhs = eval(arena, expr->as.binop.lhs);
          if (!lhs) return NULL;
          node_t* rhs = eval(arena, expr->as.binop.rhs);
          if (!rhs) return NULL;
          return node_constant(arena, expr->cursor, lhs->as.constant * rhs->as.constant);
        }
        case NO_DIV: {
          node_t* lhs = eval(arena, expr->as.binop.lhs);
          if (!lhs) return NULL;
          node_t* rhs = eval(arena, expr->as.binop.rhs);
          if (!rhs) return NULL;
          // TODO: Rethink if DIVIDE BY ZERO should be checked!
          return node_constant(arena, expr->cursor, lhs->as.constant / rhs->as.constant);
        }
        case NO_POW: {
          node_t* lhs = eval(arena, expr->as.binop.lhs);
          if (!lhs) return NULL;
          node_t* rhs = eval(arena, expr->as.binop.rhs);
          if (!rhs) return NULL;
          return node_constant(arena, expr->cursor, pow(lhs->as.constant, rhs->as.constant));
        }
        case NO_COUNT:
        default:
          UNREACHABLE("Invalid binop-node-type!");
      }
      break;
    case NT_FUNCTION:
      switch (expr->as.func.type) {
        case NF_SQRT: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, sqrt(func->as.constant));
        }
        case NF_EXP: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, exp(func->as.constant));
        }
        case NF_SIN: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, sin(func->as.constant));
        }
        case NF_ASIN: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, asin(func->as.constant));
        }
        case NF_SINH: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, sinh(func->as.constant));
        }
        case NF_COS: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, cos(func->as.constant));
        }
        case NF_ACOS: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, acos(func->as.constant));
        }
        case NF_COSH: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, cosh(func->as.constant));
        }
        case NF_TAN: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, tan(func->as.constant));
        }
        case NF_ATAN: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, atan(func->as.constant));
        }
        case NF_TANH: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, tanh(func->as.constant));
        }
        case NF_LN: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, log(func->as.constant));
        }
        case NF_LOG10: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, expr->cursor, log10(func->as.constant));
        }
        case NF_COUNT:
        default:
          UNREACHABLE("Invalid function-node-type!");
      }
      break;
    case NT_BRACKET:
      switch(expr->as.bracket.type) {
        case NB_PAREN: {
          node_t* bracket = eval(arena, expr->as.bracket.arg);
          if (!bracket) return NULL;
          return node_constant(arena, expr->cursor, bracket->as.constant);
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



node_t* parse_lexer(node_arena_t* arena, lexer_t* lexer)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lexer->isError)
    return NULL;

  // TODO:
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
