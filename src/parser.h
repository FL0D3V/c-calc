// Tries to parse a given lexed tokens-list and generates an AST for checking the order of operations in the math equation.

#ifndef _PARSER_H_
#define _PARSER_H_

#include "global.h"
#include "lexer.h"
#include "darray.h"

#include <math.h>


#define S_ERROR_NAME "semantic_error"
#define S_ERROR_EXPECTED_OPAREN(cursor)   fprintf(stderr, S_ERROR_NAME ":%zu: Expected open-parenthesis!\n", (cursor))
#define S_ERROR_EXPECTED_CPAREN(cursor)   fprintf(stderr, S_ERROR_NAME ":%zu: Expected closing-parenthesis!\n", (cursor))
#define S_ERROR_EXPECTED_OPERATOR(cursor) fprintf(stderr, S_ERROR_NAME ":%zu: Expected operator!\n", (cursor))
#define S_ERROR_UNEXPECTED_TOKEN(cursor)  fprintf(stderr, S_ERROR_NAME ":%zu: Unexpected value detected!\n", (cursor))
#define S_ERROR_EXPECTED_ARG(cursor)      fprintf(stderr, S_ERROR_NAME ":%zu: Expected arguments!\n", (cursor))
#define S_ERROR_EXPECTED_NUMBER_OR_OPAREN(cursor) \
    fprintf(stderr, S_ERROR_NAME ":%zu: Expected number or open-parenthesis!\n", (cursor))
#define E_ERROR_NAME "evaluation_error"
#define E_ERROR_DIVIDE_BY_ZERO(cursor)    fprintf(stderr, E_ERROR_NAME ":%zu: Tried to divide by zero!\n", (cursor))



typedef enum {
    NT_CONSTANT,
    NT_BINOP,
    NT_FUNCTION,
    NT_PAREN,

    NT_COUNT
} e_node_type;

static_assert(NT_COUNT == 4, "Amount of node-types have changed");
const char* nodeTypeNames[NT_COUNT] = {
  [NT_CONSTANT] = "constant",
  [NT_BINOP] = "operator",
  [NT_FUNCTION] = "function",
  [NT_PAREN] = "parenthesis"
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
  node_t* arg;
} node_paren_t;

typedef union {
  double constant;
  node_binop_t binop;
  node_function_t func;
  node_paren_t paren;
} u_node_as;

struct node {
  e_node_type type;
  size_t cursor;
  u_node_as as;
};



#define NODE_REGION_DEFAULT_CAPACITY 1024

typedef struct node_region node_region_t;

struct node_region {
  node_region_t* next;
  size_t capacity;
  size_t count;
  node_t data[];
};

typedef struct {
  node_region_t* begin;
  node_region_t* end;
} node_arena_t;


static node_region_t* node_region_alloc(size_t capacity)
{
  size_t regionSize = sizeof(node_region_t) + sizeof(node_t) * capacity;
  node_region_t* region = (node_region_t*) malloc(regionSize);
  assert(region && "Not enough memory!");
  region->next = NULL;
  region->capacity = capacity;
  region->count = 0;
  return region;
}

static void free_node_region(node_region_t* region)
{
  free(region);
}

static node_t* node_arena_alloc(node_arena_t* arena)
{
  ASSERT_NULL(arena);
  
  if (!arena->end)
  {
    assert(!arena->begin && "Begin was already set!");
    arena->end = node_region_alloc(NODE_REGION_DEFAULT_CAPACITY);
    arena->begin = arena->end;
  }

  while (arena->end->count + 1 > arena->end->capacity && arena->end->next)
    arena->end = arena->end->next;

  if (arena->end->count + 1 > arena->end->capacity)
  {
    assert(!arena->end->next && "The next after current end was already set!");
    arena->end->next = node_region_alloc(NODE_REGION_DEFAULT_CAPACITY);
    arena->end = arena->end->next;
  }

  return &arena->end->data[arena->end->count++];
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
  arena->end = NULL;
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

node_t* node_paren(node_arena_t* arena, size_t cursor, node_t* arg)
{
  node_t* node = base_node(arena, cursor, NT_PAREN);
  node->as.paren.arg = arg;
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
    {
      _PRINT_DEPTH_SPACES(indented, deph);
      printf(DOUBLE_PRINT_FORMAT, node->as.constant);
      break;
    }
    case NT_BINOP:
    {
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
    }
    case NT_FUNCTION:
    {
      if (node->as.func.type >= NF_COUNT)
        UNREACHABLE("Invalid func-node-type!");
      
      _PRINT_DEPTH_SPACES(indented, deph);
      printf("%s(", nodeFunctionTypeNames[node->as.func.type]);
      bool isArgTypeConst = node->as.func.arg->type == NT_CONSTANT;
      if (indented && !isArgTypeConst) printf("\n");
      print_node_ex(node->as.func.arg, indented, !isArgTypeConst ? deph + 1 : 0);
      if (indented && !isArgTypeConst) printf("\n");
      _PRINT_DEPTH_SPACES(indented, !isArgTypeConst ? deph : 0);
      printf(")");
      break;
    }
    case NT_PAREN:
    {
      _PRINT_DEPTH_SPACES(indented, deph);
      printf("paren(");
      bool isArgTypeConst = node->as.paren.arg->type == NT_CONSTANT;
      if (indented && !isArgTypeConst) printf("\n");
      print_node_ex(node->as.paren.arg, indented, !isArgTypeConst ? deph + 1 : 0);
      if (indented && !isArgTypeConst) printf("\n");
      _PRINT_DEPTH_SPACES(indented, !isArgTypeConst ? deph : 0);
      printf(")");
      break;
    }
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
          if (rhs->as.constant == 0)
          {
            E_ERROR_DIVIDE_BY_ZERO(rhs->cursor);
            return NULL;
          }
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
          return node_constant(arena, func->cursor, sqrt(func->as.constant));
        }
        case NF_EXP: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, exp(func->as.constant));
        }
        case NF_SIN: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, sin(func->as.constant));
        }
        case NF_ASIN: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, asin(func->as.constant));
        }
        case NF_SINH: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, sinh(func->as.constant));
        }
        case NF_COS: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, cos(func->as.constant));
        }
        case NF_ACOS: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, acos(func->as.constant));
        }
        case NF_COSH: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, cosh(func->as.constant));
        }
        case NF_TAN: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, tan(func->as.constant));
        }
        case NF_ATAN: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, atan(func->as.constant));
        }
        case NF_TANH: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, tanh(func->as.constant));
        }
        case NF_LN: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, log(func->as.constant));
        }
        case NF_LOG10: {
          node_t* func = eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, log10(func->as.constant));
        }
        case NF_COUNT:
        default:
          UNREACHABLE("Invalid function-node-type!");
      }
      break;
    case NT_PAREN:
    {
      node_t* argEval = eval(arena, expr->as.paren.arg);
      if (!argEval) return NULL;
      return node_constant(arena, expr->cursor, argEval->as.constant);
    }
    case NT_COUNT:
    default:
      UNREACHABLE("Invalid node-type!");
  }
}



static bool check_semantics(lexer_t* lexer)
{
  // TODO: It must be possible to use '-' & '+' operators infront of everything to change if the expression after
  //       them is negativ or positiv. The '+' operator though can be ignored because the expression is by default
  //       positiv.

  ASSERT_NULL(lexer);

  if (lexer->count <= 0)
    return false;
  
  bool isError = false;
  size_t parenCount = 0;

  for (size_t i = 0; i < lexer->count; ++i)
  {
    token_t* tok = &lexer->items[i];

    if (i == 0 &&
        (tok->type != TT_NUMBER &&
         tok->type != TT_MATH_CONSTANT &&
         tok->type != TT_FUNCTION &&
         (tok->type != TT_PAREN || tok->as.paren != PT_OPAREN)))
    {
      S_ERROR_EXPECTED_NUMBER_OR_OPAREN(tok->cursor);
      isError = true;
      continue;
    }

    if (i > 0 && lexer->items[i - 1].type == TT_FUNCTION &&
        (tok->type != TT_PAREN || tok->as.paren != PT_OPAREN))
    {
      S_ERROR_EXPECTED_OPAREN(tok->cursor);
      isError = true;
      continue;
    }

    switch (tok->type)
    {
      case TT_MATH_CONSTANT:
      case TT_NUMBER:
      {
        if (i <= 0)
          continue;

        token_t* lastTok = &lexer->items[i - 1];
        
        if (lastTok->type != TT_OPERATOR &&
            (lastTok->type != TT_PAREN || lastTok->as.paren != PT_OPAREN))
        {
          S_ERROR_EXPECTED_OPERATOR(lastTok->cursor);
          isError = true;
          continue;
        }

        break;
      }
      case TT_OPERATOR:
      {
        if (i <= 0)
          continue;
        
        token_t* lastTok = &lexer->items[i - 1];

        if (lastTok->type != TT_NUMBER &&
            lastTok->type != TT_MATH_CONSTANT &&
            (lastTok->type != TT_PAREN || lastTok->as.paren != PT_CPAREN))
        {
          S_ERROR_EXPECTED_NUMBER_OR_OPAREN(lastTok->cursor);
          isError = true;
          continue;
        }
        
        if (i == lexer->count - 1)
        {
          S_ERROR_UNEXPECTED_TOKEN(tok->cursor);
          isError = true;
          continue;
        }

        break;
      }
      case TT_PAREN:
      {
        e_paren_type ptype = tok->as.paren;
        
        if (ptype == PT_OPAREN)
        {
          parenCount++;
        }
        else if (ptype == PT_CPAREN)
        {
          if (parenCount == 0)
          {
            S_ERROR_UNEXPECTED_TOKEN(tok->cursor);
            isError = true;
            continue;
          }

          if (i <= 0)
            continue;

          token_t* lastTok = &lexer->items[i - 1];

          if (lastTok->type == TT_OPERATOR)
          {
            S_ERROR_UNEXPECTED_TOKEN(lastTok->cursor);
            isError = true;
            continue;
          }

          if (lastTok->type == TT_PAREN &&
              lastTok->as.paren == PT_OPAREN)
          {
            S_ERROR_EXPECTED_ARG(lastTok->cursor);
            isError = true;
            continue;
          }

          parenCount--;
        }
        else
          UNREACHABLE("Not implemented!");

        if (i == lexer->count - 1)
        {
          if (parenCount > 0)
          {
            S_ERROR_EXPECTED_CPAREN(tok->cursor);
            isError = true;
            continue;
          }
        }

        break;
      }
      case TT_FUNCTION:
      {
        if (i <= 0)
          continue;

        token_t* lastTok = &lexer->items[i - 1];

        if (lastTok->type != TT_OPERATOR &&
            (lastTok->type != TT_PAREN || lastTok->as.paren != PT_OPAREN))
        {
          S_ERROR_EXPECTED_OPERATOR(lastTok->cursor);
          isError = true;
          continue;
        }

        if (i == lexer->count - 1)
        {
          S_ERROR_UNEXPECTED_TOKEN(tok->cursor);
          isError = true;
          continue;
        }

        break;
      }
      case TT_COUNT:
      default:
        UNREACHABLE("Invalid token-type!");
    }
  }

  return isError;
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

  if (!check_semantics(lexer))
    return NULL;

  //for (size_t i = 0; i < lexer->count; ++i)
  //{
  //  break;
  //}

  // TODO: Return root node!
  return NULL;  
}

#endif // _PARSER_H_
