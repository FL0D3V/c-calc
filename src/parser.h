#ifndef _PARSER_H_
#define _PARSER_H_

#include <math.h>

#include "lexer.h"


// Errors
#define _S_ERROR_NAME "SEMANTIC-ERROR"
#define _E_ERROR_NAME "EVALUATION-ERROR"
#define _PARSER_ERROR(type, cursor, message) fprintf(stderr, type ":%zu: %s\n", (cursor), (message));
#define S_ERROR(cursor, message) _PARSER_ERROR(_S_ERROR_NAME, cursor, message)
#define E_ERROR(cursor, message) _PARSER_ERROR(_E_ERROR_NAME, cursor, message)


// All enums
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

static inline e_node_binop_type to_local_binop_type(e_operator_type type)
{
  switch (type)
  {
    case OP_ADD: return NO_ADD;
    case OP_SUB: return NO_SUB;
    case OP_MUL: return NO_MUL;
    case OP_DIV: return NO_DIV;
    case OP_POW: return NO_POW;
    case OP_COUNT:
    case OP_INVALID:
    default: UNREACHABLE("Operator-Type not implemented!");
  }
}


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

static inline e_node_func_type to_local_func_type(e_function_type type)
{
  switch (type)
  {
    case FT_SQRT: return NF_SQRT;
    case FT_EXP: return NF_EXP;
    case FT_SIN: return NF_SIN;
    case FT_ASIN: return NF_ASIN;
    case FT_SINH: return NF_SINH;
    case FT_COS: return NF_COS;
    case FT_ACOS: return NF_ACOS;
    case FT_COSH: return NF_COSH;
    case FT_TAN: return NF_TAN;
    case FT_ATAN: return NF_ATAN;
    case FT_TANH: return NF_TANH;
    case FT_LN: return NF_LN;
    case FT_LOG10: return NF_LOG10;
    case FT_COUNT:
    case FT_INVALID:
    default: UNREACHABLE("Function-Type not implemented!");
  }
}


// Node definitions
typedef struct node node_t;

typedef struct {
  e_node_binop_type type;
  node_t* lhs;
  node_t* rhs;
} node_binop_t;

// TODO: Rethink! When implementing multiple function args, change arg -> args (dynamic array).
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



// Creating new nodes
static node_t* base_node(arena_t* arena, size_t cursor, e_node_type type)
{
  node_t* newNode = (node_t*) arena_alloc((arena), sizeof(node_t));
  newNode->type = type;
  newNode->cursor = cursor;
  return newNode;
}

node_t* node_constant(arena_t* arena, size_t cursor, double constant)
{
  node_t* node = base_node(arena, cursor, NT_CONSTANT);
  node->as.constant = constant;
  return node;
}

node_t* node_binop(arena_t* arena, size_t cursor, e_node_binop_type type, node_t* lhs, node_t* rhs)
{
  node_t* node = base_node(arena, cursor, NT_BINOP);
  node->as.binop.type = type;
  node->as.binop.lhs = lhs;
  node->as.binop.rhs = rhs;
  return node;
}

node_t* node_func(arena_t* arena, size_t cursor, e_node_func_type type, node_t* arg)
{
  node_t* node = base_node(arena, cursor, NT_FUNCTION);
  node->as.func.type = type;
  node->as.func.arg = arg;
  return node;
}

node_t* node_paren(arena_t* arena, size_t cursor, node_t* arg)
{
  node_t* node = base_node(arena, cursor, NT_PAREN);
  node->as.paren.arg = arg;
  return node;
}



node_t* ast_eval(arena_t* arena, node_t* expr)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(expr);

  switch (expr->type)
  {
    case NT_CONSTANT:
      return expr;
    case NT_BINOP:
    {
      node_t* lhs = ast_eval(arena, expr->as.binop.lhs);
      if (!lhs) return NULL;
      node_t* rhs = ast_eval(arena, expr->as.binop.rhs);
      if (!rhs) return NULL;

      switch (expr->as.binop.type)
      {
        case NO_ADD: return node_constant(arena, expr->cursor, lhs->as.constant + rhs->as.constant);
        case NO_SUB: return node_constant(arena, expr->cursor, lhs->as.constant - rhs->as.constant);
        case NO_MUL: return node_constant(arena, expr->cursor, lhs->as.constant * rhs->as.constant);
        case NO_DIV:
        {
          if (rhs->as.constant == 0)
          {
            E_ERROR(rhs->cursor, "Tried to divide by zero!");
            return NULL;
          }
          return node_constant(arena, expr->cursor, lhs->as.constant / rhs->as.constant);
        }
        case NO_POW: return node_constant(arena, expr->cursor, pow(lhs->as.constant, rhs->as.constant));
        case NO_COUNT:
        default:
          UNREACHABLE("Invalid binop-node-type!");
      }
      break;
    }
    case NT_FUNCTION:
      // Functions could support different numbers of arguments in the future.
      switch (expr->as.func.type)
      {
        case NF_SQRT:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, sqrt(func->as.constant));
        }
        case NF_EXP:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, exp(func->as.constant));
        }
        case NF_SIN:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, sin(func->as.constant));
        }
        case NF_ASIN:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, asin(func->as.constant));
        }
        case NF_SINH:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, sinh(func->as.constant));
        }
        case NF_COS:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, cos(func->as.constant));
        }
        case NF_ACOS:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, acos(func->as.constant));
        }
        case NF_COSH:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, cosh(func->as.constant));
        }
        case NF_TAN:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, tan(func->as.constant));
        }
        case NF_ATAN:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, atan(func->as.constant));
        }
        case NF_TANH:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, tanh(func->as.constant));
        }
        case NF_LN:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
          if (!func) return NULL;
          return node_constant(arena, func->cursor, log(func->as.constant));
        }
        case NF_LOG10:
        {
          node_t* func = ast_eval(arena, expr->as.func.arg);
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
      node_t* argEval = ast_eval(arena, expr->as.paren.arg);
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
  ASSERT_NULL(lexer);

  if (lexer->count <= 0)
    return false;
  
  bool isError = false;
  size_t parenCount = 0;
  
  for (size_t i = 0; i < lexer->count; ++i)
  {
    token_t* tok = lex_at(lexer, i);
    
    switch (tok->type)
    {
      case TT_MATH_CONSTANT:
      case TT_NUMBER:
      {
        if (i > 0)
        {
          token_t* lastTok = lex_at(lexer, i - 1);
          
          if (tok_is(lastTok, TT_OPERATOR) ||
              tok_is_paren(lastTok, PT_OPAREN))
            continue;

          S_ERROR(tok->cursor, "Expected an operator or an open paren before a number or constant!");
          isError = true;
          continue;
        }

        continue;
      }
      case TT_OPERATOR:
      {
        // Checks if this is the last token (the last must not be an opeartor).
        if (!lex_next_in_range(lexer, i))
        {
          S_ERROR(tok->cursor, "An operator can't be the last token!");
          isError = true;
          continue;
        }
        
        token_t* nextTok = lex_at(lexer, i + 1);

        if (i > 0)
        {
          token_t* lastTok = lex_at(lexer, i - 1);
          
          // Checks if the last token was a number or a closing paren.
          if (tok_is(lastTok, TT_NUMBER) ||
              tok_is(lastTok, TT_MATH_CONSTANT) ||
              tok_is_paren(lastTok, PT_CPAREN))
            continue;

          // Checks for ".. ')' OPERATOR NUMBER ..".
          if (tok_is_paren(lastTok, PT_CPAREN) &&
              tok_is(nextTok, TT_NUMBER))
            continue;

          // Checks for ".. '(' '+'/'-' NUMBER ..".
          if (tok_is_paren(lastTok, PT_OPAREN) &&
              tok_is_number_operator(tok) &&
              tok_is(nextTok, TT_NUMBER))
            continue;

          // Checks if last token was an operator, the current is a number operator and the next is a number ".. OPERATOR '+'/'-' NUMBER ..".
          if (tok_is(lastTok, TT_OPERATOR) &&
              tok_is_number_operator(tok) &&
              tok_is(nextTok, TT_NUMBER))
            continue;
        }

        if (tok_is_number_operator(tok) && tok_is(nextTok, TT_NUMBER))
          continue;

        S_ERROR(tok->cursor, "Invalid usage of an operator!");
        isError = true;
        continue;
      }
      case TT_PAREN:
      {
        e_paren_type ptype = tok->as.paren;

        if (ptype == PT_OPAREN)
        {
          parenCount++;
          
          if (i > 0)
          {
            token_t* lastTok = lex_at(lexer, i - 1);

            if (tok_is_paren(lastTok, PT_CPAREN))
            {
              S_ERROR(tok->cursor, "Expected operator! Before an open paren must NOT be a closing paren.");
              isError = true;
              continue;
            }

            if (tok_not(lastTok, TT_OPERATOR) && !tok_is_paren(lastTok, PT_OPAREN))
            {
              S_ERROR(tok->cursor, "Expected operator or open paren!");
              isError = true;
              continue;
            }
          }
        }
        else if (ptype == PT_CPAREN)
        {
          if (parenCount <= 0)
          {
            S_ERROR(tok->cursor, "Too many closing parens!");
            isError = true;
            continue;
          }
          
          parenCount--;

          if (i > 0)
          {
            token_t* lastTok = lex_at(lexer, i - 1);

            if (tok_is(lastTok, TT_OPERATOR))
            {
              S_ERROR(tok->cursor, "Expected an expression after an operator but got a closing paren!");
              isError = true;
              continue;
            }

            if (tok_is_paren(lastTok, PT_OPAREN))
            {
              S_ERROR(tok->cursor, "Expected an argument expression inside the parens!");
              isError = true;
              continue;
            }
          }
        }

        if (!lex_next_in_range(lexer, i) && parenCount > 0)
        {
          S_ERROR(tok->cursor, "Expected closing paren!");
          isError = true;
          continue;
        }

        continue;
      }
      case TT_FUNCTION:
      {
        // Checks that after the current initializer there are still at least 3 more tokens because a function needs 
        // an open paren, at least a single argument and a closing paren.
        if (!lex_next_in_range(lexer, i + 2))
        {
          S_ERROR(tok->cursor, "A function initializer can't be the last token because it needs an open and a closing paren and an argument expression inside them!");
          isError = true;
          continue;
        }

        token_t* nextTok = lex_at(lexer, i + 1);

        // Checks if last token was a function initializer and also if the current is an open paren ("FUNC(<-...)").
        if (tok_not_specific_paren(nextTok, PT_OPAREN))
        {
          S_ERROR(nextTok->cursor, "Expected an open paren after a function initializer!");
          isError = true;
          continue;
        }

        if (i > 0)
        {
          token_t* lastTok = lex_at(lexer, i - 1);

          // Checks if the last token was an operator or an open paren.
          if (tok_not(lastTok, TT_OPERATOR) &&
              tok_not_specific_paren(lastTok, PT_OPAREN))
          {
            S_ERROR(lastTok->cursor, "Before a function initializer must be an operator or an open paren!");
            isError = true;
            continue;
          }
        }

        // This ignores the cheking for the next token because the checks here make sure that the next is an open paren.
        parenCount++;
        i++;

        continue;
      }
      case TT_LITERAL:
      {
        // TODO: Implement!
        // Current literals:
        // > ',': for multi argument functions like '... funcTest(arg1, arg2, arg3) ...'.
        // > '=': for equations like '10 + 5 = 20 - 5'. This could return f.e. 'true' or 'false'.
        //        Also it could maybe be used for assigning an expression to a variable.

        S_ERROR(tok->cursor, "Literal not implemented yet!");
        isError = true; // TODO: Rethink!
        continue;
      }
      case TT_COUNT:
      default:
        UNREACHABLE("Invalid token-type!");
    }
  }

  if (parenCount != 0 && !isError)
  {
    S_ERROR(lex_at(lexer, lexer->count - 1)->cursor, "Invalid paren usage!");
    isError = true;
  }

  return isError;
}



static node_t* try_parse_constant(arena_t* arena, lexer_t* lexer, size_t index);
static node_t* try_parse_binop(arena_t* arena, lexer_t* lexer, size_t index);
static node_t* try_parse_func(arena_t* arena, lexer_t* lexer, size_t index);
static node_t* try_parse_paren(arena_t* arena, lexer_t* lexer, size_t index);


static node_t* try_parse_constant(arena_t* arena, lexer_t* lexer, size_t index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t* token = lex_at(lexer, index);
  
  if (tok_is(token, TT_NUMBER))
    return node_constant(arena, token->cursor, token->as.number);
  else if (tok_is(token, TT_MATH_CONSTANT))
    return node_constant(arena, token->cursor, mathConstantTypeValues[token->as.constant]);
  else return NULL;
}

static node_t* try_parse_binop(arena_t* arena, lexer_t* lexer, size_t index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t* token = lex_at(lexer, index);

  if (tok_is(token, TT_OPERATOR))
  {
    // TODO: Implement! Parse left and right argument tree.
    return NULL;
  }
  else return NULL;
}

static node_t* try_parse_func(arena_t* arena, lexer_t* lexer, size_t index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t* token = lex_at(lexer, index);

  if (tok_is(token, TT_FUNCTION))
  {
    node_t* arg = try_parse_paren(arena, lexer, index + 1);
    if (!arg) return NULL;
    return node_func(arena, token->cursor, to_local_func_type(token->as.function), arg);
  }
  else return NULL;
}

static node_t* try_parse_paren(arena_t* arena, lexer_t* lexer, size_t index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t* token = lex_at(lexer, index);

  if (tok_is(token, TT_PAREN))
  {
    // TODO: Implement! Parse everything till the till the corresponding closing paren.
    // Paren-Arguments can contain sub parens, functions, etc.
    return NULL;
  }
  else return NULL;
}



node_t* parser_execute(arena_t* arena, lexer_t* lexer)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lexer->isError || lexer->count <= 0)
    return NULL;

  printf("INFO: AST generator and parser is currently under developement.\n");

  // TODO:
  // The parser needs to reed to the next operator, paren, or func for every node.
  // Order checking: Brackets -> Exponents -> Multiplication -> Division -> Addition -> Substraction
  // A stack for checking nested expressions in brackets. Also if the brackets are used correctly.
  // Paren checking: 2 Open '(' should have 2 Closing ')' after.
  // After a constant / nubmer MUST come an operator, function, or closing paren
  
  if (!check_semantics(lexer))
    return NULL;

  // IN: "EN + 5 / (5 * 0)"
  // AST: "add(EN, divide(5, paren(mult(5, 0))))"
  // Getting constants - global.h: mathConstantTypeValues[token->as.constant]

  node_t* root = NULL;

  /*for (size_t i = 0; i < lexer->count; ++i)
  {
    token_t* tok = lex_at(lexer, i);

    // TODO: Implement!
    (void) tok;

    break;

    switch (tok->type)
    {

      case TT_COUNT:
      default:
        UNREACHABLE("Invalid token-type!");
    }
  }*/

  // TODO: Return root node!
  return root;
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


void print_node(node_t* node, bool indented)
{
  printf("Printing parsed AST:\n");
  print_node_ex(node, indented, 0);
  printf("\n");
}

#endif // _PARSER_H_
