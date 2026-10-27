#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"

#define _S_ERROR_NAME "SEMANTIC-ERROR"
#define _E_ERROR_NAME "EVALUATION-ERROR"
#define _PARSER_ERROR(type, cursor, message) \
  fprintf(stderr, type ":%zu: %s\n", (cursor), (message));
#define S_ERROR(cursor, message) _PARSER_ERROR(_S_ERROR_NAME, cursor, message)
#define E_ERROR(cursor, message) _PARSER_ERROR(_E_ERROR_NAME, cursor, message)

typedef enum
{
  NT_CONSTANT,
  NT_BINOP,
  NT_FUNCTION,
  NT_PAREN,

  NT_COUNT
} e_node_type;

static_assert(NT_COUNT == 4, "Amount of node-types have changed");

const char *nodeTypeNames[NT_COUNT] = {
    [NT_CONSTANT] = "constant",
    [NT_BINOP] = "operator",
    [NT_FUNCTION] = "function",
    [NT_PAREN] = "parenthesis"};

typedef enum
{
  NO_ADD,
  NO_SUB,
  NO_MUL,
  NO_DIV,
  NO_POW,

  NO_COUNT
} e_node_binop_type;

static_assert(NO_COUNT == 5, "Amount of binop-node-types have changed");

const char *nodeBinopTypeNames[NO_COUNT] = {
    [NO_ADD] = "add",
    [NO_SUB] = "substract",
    [NO_MUL] = "multiply",
    [NO_DIV] = "divide",
    [NO_POW] = "pow"};

static inline e_node_binop_type to_local_binop_type(e_operator_type type)
{
  switch (type)
  {
  case OP_ADD:
    return NO_ADD;
  case OP_SUB:
    return NO_SUB;
  case OP_MUL:
    return NO_MUL;
  case OP_DIV:
    return NO_DIV;
  case OP_POW:
    return NO_POW;
  case OP_COUNT:
  case OP_INVALID:
  default:
    UNREACHABLE("Operator-Type not implemented!");
  }
}

typedef enum
{
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

const char *nodeFunctionTypeNames[NF_COUNT] = {
    [NF_SQRT] = "sqrt",
    [NF_EXP] = "exp",

    [NF_SIN] = "sin",
    [NF_ASIN] = "asin",
    [NF_SINH] = "sinh",

    [NF_COS] = "cos",
    [NF_ACOS] = "acos",
    [NF_COSH] = "cosh",

    [NF_TAN] = "tan",
    [NF_ATAN] = "atan",
    [NF_TANH] = "tanh",

    [NF_LN] = "ln",
    [NF_LOG10] = "log10"};

static inline e_node_func_type to_local_func_type(e_function_type type)
{
  switch (type)
  {
  case FT_SQRT:
    return NF_SQRT;
  case FT_EXP:
    return NF_EXP;
  case FT_SIN:
    return NF_SIN;
  case FT_ASIN:
    return NF_ASIN;
  case FT_SINH:
    return NF_SINH;
  case FT_COS:
    return NF_COS;
  case FT_ACOS:
    return NF_ACOS;
  case FT_COSH:
    return NF_COSH;
  case FT_TAN:
    return NF_TAN;
  case FT_ATAN:
    return NF_ATAN;
  case FT_TANH:
    return NF_TANH;
  case FT_LN:
    return NF_LN;
  case FT_LOG10:
    return NF_LOG10;
  case FT_COUNT:
  case FT_INVALID:
  default:
    UNREACHABLE("Function-Type not implemented!");
  }
}

// Node definitions
typedef struct node node_t;

typedef struct
{
  e_node_binop_type type;
  node_t *lhs;
  node_t *rhs;
} node_binop_t;

typedef struct
{
  node_t **items;
  size_t capacity;
  size_t count;
} node_func_args_t;

#define node_func_args_append(a, args, item) \
  arena_da_append(a, args, item);

typedef struct
{
  e_node_func_type type;
  node_func_args_t args;
} node_function_t;

typedef struct
{
  node_t *arg;
} node_paren_t;

typedef union
{
  // TODO: Change to big number type!
  double constant;
  node_binop_t binop;
  node_function_t func;
  node_paren_t paren;
} u_node_as;

struct node
{
  e_node_type type;
  size_t cursor;
  u_node_as as;
};

// Creating new nodes
static node_t *base_node(arena_t *arena, size_t cursor, e_node_type type)
{
  node_t *newNode = (node_t *)arena_alloc((arena), sizeof(node_t));
  newNode->type = type;
  newNode->cursor = cursor;
  return newNode;
}

// TODO: Change to big number type!
node_t *node_constant(arena_t *arena, size_t cursor, double constant)
{
  node_t *node = base_node(arena, cursor, NT_CONSTANT);
  node->as.constant = constant;
  return node;
}

node_t *node_binop(arena_t *arena, size_t cursor, e_node_binop_type type, node_t *lhs, node_t *rhs)
{
  node_t *node = base_node(arena, cursor, NT_BINOP);
  node->as.binop.type = type;
  node->as.binop.lhs = lhs;
  node->as.binop.rhs = rhs;
  return node;
}

node_t *node_func(arena_t *arena, size_t cursor, e_node_func_type type, node_func_args_t args)
{
  node_t *node = base_node(arena, cursor, NT_FUNCTION);
  node->as.func.type = type;
  node->as.func.args = args;
  return node;
}

node_t *node_paren(arena_t *arena, size_t cursor, node_t *arg)
{
  node_t *node = base_node(arena, cursor, NT_PAREN);
  node->as.paren.arg = arg;
  return node;
}

static bool check_semantics(lexer_t *lexer)
{
  ASSERT_NULL(lexer);

  if (lexer->count <= 0)
    return false;

  bool isError = false;
  size_t parenCount = 0;

  for (size_t i = 0; i < lexer->count; ++i)
  {
    token_t *tok = lex_at(lexer, i);

    switch (tok->type)
    {
    case TT_MATH_CONSTANT:
    case TT_NUMBER:
    {
      if (i > 0)
      {
        token_t *lastTok = lex_at(lexer, i - 1);

        if (tok_is(lastTok, TT_OPERATOR) ||
            tok_is_specific_literal(lastTok, CLT_EQUALS) ||
            tok_is_paren(lastTok, PT_OPAREN))
          continue;

        S_ERROR(tok->cursor, "Unexpected token before the number!");
        isError = true;
        continue;
      }

      // Valid number placement.
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

      token_t *nextTok = lex_at(lexer, i + 1);

      if (i > 0)
      {
        token_t *lastTok = lex_at(lexer, i - 1);

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
          token_t *lastTok = lex_at(lexer, i - 1);

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
          token_t *lastTok = lex_at(lexer, i - 1);

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

      token_t *nextTok = lex_at(lexer, i + 1);

      // Checks if last token was a function initializer and also if the current is an open paren ("FUNC(<-...)").
      if (tok_not_specific_paren(nextTok, PT_OPAREN))
      {
        S_ERROR(nextTok->cursor, "Expected an open paren after a function initializer!");
        isError = true;
        continue;
      }

      if (i > 0)
      {
        token_t *lastTok = lex_at(lexer, i - 1);

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
      //        Also it could be used for assigning an expression to a variable or creating a new function.
      //        Like:
      //            'test = 10 + 5'
      //            'test(x, y) = 20 * 5 - sqrt(x) ^ y'

      switch (tok->as.literal)
      {
      case CLT_COMMA:
      {
        if (i <= 0)
        {
          S_ERROR(tok->cursor, "This literal can't be at the start!");
          isError = true;
          continue;
        }

        token_t *lastTok = lex_at(lexer, i + 1);

        if (tok_is(lastTok, TT_LITERAL) ||
            tok_is(lastTok, TT_FUNCTION) ||
            tok_is_paren(lastTok, PT_OPAREN) ||
            tok_is(lastTok, TT_OPERATOR) ||
            tok_is(lastTok, TT_SYMBOL))
        {
          S_ERROR(tok->cursor, "Before this literal there must be a valid argument!");
          isError = true;
          continue;
        }

        if (!lex_next_in_range(lexer, i + 1))
        {
          S_ERROR(tok->cursor, "This literal must have a valid argument after it!");
          isError = true;
          continue;
        }

        token_t *nextTok = lex_at(lexer, i + 1);

        if (tok_is(nextTok, TT_LITERAL) ||
            tok_is(nextTok, TT_SYMBOL) ||
            tok_is_paren(nextTok, PT_CPAREN) ||
            tok_is(nextTok, TT_OPERATOR))
        {
          S_ERROR(tok->cursor, "After this literal must be a valid argument!");
          isError = true;
          continue;
        }

        // TODO: Implement the checks to make sure this is only used inside a function definition!

        break;
      }
      case CLT_EQUALS:
      {
        if (i <= 0)
        {
          S_ERROR(tok->cursor, "A variable assignment literal can't be at the start!");
          isError = true;
          continue;
        }

        token_t *lastTok = lex_at(lexer, i - 1);

        // Checks if the last token was an operator or an open paren.
        if (tok_not(lastTok, TT_SYMBOL))
        {
          S_ERROR(lastTok->cursor, "Before a variable assignment literal must be a variable name symbol!");
          isError = true;
          continue;
        }

        continue;
      }
      case CLT_COUNT:
      case CLT_INVALID:
      default:
        UNREACHABLE("Unreachable literal type!");
      }

      S_ERROR(tok->cursor, "Literal not implemented yet!");
      isError = true;
      continue;
    }
    case TT_SYMBOL:
    {
      if (i != 0)
      {
        S_ERROR(tok->cursor, "Variable name definition must be at the start!");
        isError = true;
        continue;
      }

      // Checks that after the current initializer there are still at least 2 more tokens because a variable definition needs
      // an variable definition literal and at least a single argument.
      if (!lex_next_in_range(lexer, i + 2))
      {
        S_ERROR(tok->cursor, "A variable name definition can't be the last token because it needs a variable assignment literal and at least a single argument!");
        isError = true;
        continue;
      }

      token_t *nextTok = lex_at(lexer, i + 1);

      if (tok_not_specific_literal(nextTok, CLT_EQUALS))
      {
        S_ERROR(nextTok->cursor, "Expected a variable assignment literal after variable name definition!");
        isError = true;
        continue;
      }

      ++i;

      nextTok = lex_at(lexer, i + 1);

      // Valid variable name definition.
      if (tok_is(nextTok, TT_NUMBER) ||
          tok_is(nextTok, TT_MATH_CONSTANT) ||
          tok_is(nextTok, TT_FUNCTION) ||
          tok_is_paren(nextTok, PT_OPAREN))
        continue;

      S_ERROR(nextTok->cursor, "Expected a valid argument after variable assignment literal!");
      isError = true;
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

static node_t *try_parse_constant(arena_t *arena, lexer_t *lexer, size_t index);
static node_t *try_parse_binop(arena_t *arena, lexer_t *lexer, size_t index);
static node_t *try_parse_func(arena_t *arena, lexer_t *lexer, size_t index);
static node_t *try_parse_paren(arena_t *arena, lexer_t *lexer, size_t index);

static node_t *try_parse_constant(arena_t *arena, lexer_t *lexer, size_t index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t *token = lex_at(lexer, index);

  if (tok_is(token, TT_NUMBER))
    return node_constant(arena, token->cursor, token->as.number);
  else if (tok_is(token, TT_MATH_CONSTANT))
    return node_constant(arena, token->cursor, mathConstantTypeValues[token->as.constant]);
  else
    return NULL;
}

static node_t *try_parse_binop(arena_t *arena, lexer_t *lexer, size_t index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t *token = lex_at(lexer, index);

  if (tok_is(token, TT_OPERATOR))
  {
    // TODO: Implement! Parse left and right argument tree.
    return NULL;
  }
  else
    return NULL;
}

static node_t *try_parse_func(arena_t *arena, lexer_t *lexer, size_t index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t *token = lex_at(lexer, index);

  if (tok_is(token, TT_FUNCTION))
  {
    // TODO: Make sure that all function arguments get loaded.
    node_t *arg = try_parse_paren(arena, lexer, index + 1);
    if (!arg)
      return NULL;

    // TODO: Rethink!
    node_func_args_t argsList = {0};
    node_func_args_append(arena, &argsList, arg);

    return node_func(arena, token->cursor, to_local_func_type(token->as.function), argsList);
  }
  else
    return NULL;
}

static node_t *try_parse_paren(arena_t *arena, lexer_t *lexer, size_t index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t *token = lex_at(lexer, index);

  if (tok_is(token, TT_PAREN))
  {
    // TODO: Implement! Parse everything  till the corresponding closing paren.
    // Paren-Arguments can contain sub parens, functions, etc.
    return NULL;
  }
  else
    return NULL;
}

node_t *parser_execute(arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lexer->isError || lexer->count <= 0)
    return NULL;

  // TODO:
  // The parser needs to reed to the next operator, paren, or func for every node.
  // Order checking: Brackets -> Exponents -> Multiplication / Division -> Addition / Substraction
  // A stack for checking nested expressions in brackets. Also if the brackets are used correctly.
  // Paren checking: 2 Open '(' should have 2 Closing ')' after.
  // After a constant / number MUST come an operator, function, or closing paren

  fprintf(stderr, "ERROR: Parser is currently under developement!\n");

  // TODO: Rethink!
  if (!check_semantics(lexer))
    return NULL;

  // IN: "EN + 5 / (5 * 0)"
  // AST: "add(EN, divide(5, paren(mult(5, 0))))"

  node_t *root = NULL;

  return root;
}

node_t *ast_eval_ex(arena_t *arena, node_t *expr)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(expr);

  switch (expr->type)
  {
  case NT_CONSTANT:
    return expr;
  case NT_BINOP:
  {
    node_t *lhs = ast_eval_ex(arena, expr->as.binop.lhs);
    if (!lhs)
      return NULL;
    node_t *rhs = ast_eval_ex(arena, expr->as.binop.rhs);
    if (!rhs)
      return NULL;

    switch (expr->as.binop.type)
    {
    case NO_ADD:
      return node_constant(arena, expr->cursor, lhs->as.constant + rhs->as.constant);
    case NO_SUB:
      return node_constant(arena, expr->cursor, lhs->as.constant - rhs->as.constant);
    case NO_MUL:
      return node_constant(arena, expr->cursor, lhs->as.constant * rhs->as.constant);
    case NO_DIV:
    {
      if (rhs->as.constant == 0)
      {
        E_ERROR(rhs->cursor, "Tried to divide by zero!");
        return NULL;
      }
      return node_constant(arena, expr->cursor, lhs->as.constant / rhs->as.constant);
    }
    case NO_POW:
      return node_constant(arena, expr->cursor, pow(lhs->as.constant, rhs->as.constant));
    case NO_COUNT:
    default:
      UNREACHABLE("Invalid binop-node-type!");
    }
    break;
  }
  case NT_FUNCTION:
  {
    // TODO: Functions could support different numbers of arguments in the future.
    switch (expr->as.func.type)
    {
    case NF_SQRT:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "sqrt() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, sqrt(func->as.constant));
    }
    case NF_EXP:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "exp() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, exp(func->as.constant));
    }
    case NF_SIN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "sin() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, sin(func->as.constant));
    }
    case NF_ASIN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "asin() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, asin(func->as.constant));
    }
    case NF_SINH:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "sinh() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, sinh(func->as.constant));
    }
    case NF_COS:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "cos() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, cos(func->as.constant));
    }
    case NF_ACOS:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "acos() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, acos(func->as.constant));
    }
    case NF_COSH:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "cosh() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, cosh(func->as.constant));
    }
    case NF_TAN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "tanh() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, tan(func->as.constant));
    }
    case NF_ATAN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "atan() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, atan(func->as.constant));
    }
    case NF_TANH:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "tanh() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, tanh(func->as.constant));
    }
    case NF_LN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "ln() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, log(func->as.constant));
    }
    case NF_LOG10:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "log10() takes one argument!");
        return NULL;
      }
      node_t *func = ast_eval_ex(arena, expr->as.func.args.items[0]);
      if (!func)
        return NULL;
      return node_constant(arena, func->cursor, log10(func->as.constant));
    }
    case NF_COUNT:
    default:
      UNREACHABLE("Invalid function-node-type!");
    }
    break;
  }
  case NT_PAREN:
  {
    node_t *argEval = ast_eval_ex(arena, expr->as.paren.arg);
    if (!argEval)
      return NULL;
    return node_constant(arena, expr->cursor, argEval->as.constant);
  }
  case NT_COUNT:
  default:
    UNREACHABLE("Invalid node-type!");
  }
}

typedef struct
{
  bool error;
  // TODO: Change to big number!
  double value;
} ast_eval_res_t;

ast_eval_res_t ast_eval(arena_t *arena, node_t *expr)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(expr);

  ast_eval_res_t result = {0};

  node_t *evaluatedNode = ast_eval_ex(arena, expr);
  if (!evaluatedNode)
  {
    E_ERROR(expr->cursor, "Evaluation failed!");
    result.error = true;
    return result;
  }

  if (evaluatedNode->type != NT_CONSTANT)
  {
    E_ERROR(expr->cursor, "Evaluation failed! Return-Type not of type 'constant'!");
    result.error = true;
    return result;
  }

  result.value = evaluatedNode->as.constant;
  return result;
}

#define _PRINT_DEPTH_SPACES(indented, deph) \
  do                                        \
  {                                         \
    if (indented)                           \
      for (size_t i = 0; i < (deph); ++i)   \
        printf(" ");                        \
  } while (0)

void print_node_ex(node_t *node, bool indented, size_t deph)
{
  ASSERT_NULL(node);

  switch (node->type)
  {
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
    if (indented)
      printf("\n");
    print_node_ex(node->as.binop.lhs, indented, deph + 1);
    printf(",%s", indented ? "\n" : " ");
    print_node_ex(node->as.binop.rhs, indented, deph + 1);
    if (indented)
      printf("\n");
    _PRINT_DEPTH_SPACES(indented, deph);
    printf(")");
    break;
  }
  case NT_FUNCTION:
  {
    // TODO: Implement multi argument function printing!
    if (node->as.func.type >= NF_COUNT)
      UNREACHABLE("Invalid func-node-type!");

    // TODO: Make sure that all function args get displayed!
    _PRINT_DEPTH_SPACES(indented, deph);
    printf("%s(", nodeFunctionTypeNames[node->as.func.type]);
    bool isArgTypeConst = node->as.func.args.items[0]->type == NT_CONSTANT;
    if (indented && !isArgTypeConst)
      printf("\n");
    print_node_ex(node->as.func.args.items[0], indented, !isArgTypeConst ? deph + 1 : 0);
    if (indented && !isArgTypeConst)
      printf("\n");
    _PRINT_DEPTH_SPACES(indented, !isArgTypeConst ? deph : 0);
    printf(")");
    break;
  }
  case NT_PAREN:
  {
    _PRINT_DEPTH_SPACES(indented, deph);
    printf("paren(");
    bool isArgTypeConst = node->as.paren.arg->type == NT_CONSTANT;
    if (indented && !isArgTypeConst)
      printf("\n");
    print_node_ex(node->as.paren.arg, indented, !isArgTypeConst ? deph + 1 : 0);
    if (indented && !isArgTypeConst)
      printf("\n");
    _PRINT_DEPTH_SPACES(indented, !isArgTypeConst ? deph : 0);
    printf(")");
    break;
  }
  case NT_COUNT:
  default:
    UNREACHABLE("Invalid node-type!");
  }
}

void print_node(node_t *node, bool indented)
{
  print_node_ex(node, indented, 0);
  printf("\n");
}

typedef struct
{
  const char *inputExpression;
  double expectedResult;
  bool shouldFail;
  node_t *expressionTree;
} node_test_t;

bool execute_test(arena_t *arena, node_test_t *test, bool verbose, bool freeArena)
{
  if (verbose)
  {
    printf("\n");
    print_node(test->expressionTree, true);
    printf("\n");
  }

  printf("Input       = %s\n", test->inputExpression);
  if (!test->shouldFail)
    printf("Expected    = " DOUBLE_PRINT_FORMAT "\n", test->expectedResult);

  node_t *evaluatedNode = ast_eval_ex(arena, test->expressionTree);

  if (test->shouldFail)
  {
    if (evaluatedNode)
    {
      fprintf(stderr, "ERROR: Evaluation should have failed but did not!\n");

      if (freeArena)
        arena_free(arena);

      return false;
    }

    printf("Evaluation failed as expected.\n");
  }
  else
  {
    if (!evaluatedNode)
    {
      fprintf(stderr, "ERROR: Evaluation failed!\n");

      if (freeArena)
        arena_free(arena);

      return false;
    }

    double result = evaluatedNode->as.constant;

    if (freeArena)
      arena_free(arena);

    printf("Result      = " DOUBLE_PRINT_FORMAT "\n", result);

    if (!compare_doubles(result, test->expectedResult, 5))
    {
      fprintf(stderr, "ERROR: Evaluated result not equal to expected value!\n");
      return false;
    }
  }

  return true;
}

#endif // _PARSER_H_
