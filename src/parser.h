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
    [NT_PAREN] = "parenthesis",
};

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
    [NO_POW] = "pow",
};

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
  NF_ATAN2,

  NF_LN,
  NF_LOG10,

  NF_COUNT
} e_node_func_type;

static_assert(NF_COUNT == 14, "Amount of function-node-types have changed");

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
    [NF_ATAN2] = "atan2",

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
  case FT_ATAN2:
    return NF_ATAN2;
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

#define node_func_args_append(a, args, arg) \
  arena_da_append(a, args, arg)

typedef struct
{
  e_node_func_type type;
  node_func_args_t args;
} node_function_t;

typedef struct
{
  node_t *arg;
} node_paren_t;

// TODO: Rethink! Functions must be later also used similar.
// This means there can be build in functions and user defined functions.
typedef enum
{
  CN_VALUE,
  CN_INBUILD_CONST,
  CN_VARIABLE,

  CN_COUNT
} e_const_node_type;

static_assert(CN_COUNT == 3, "Amount of value node types have changed");

const char *constNodeTypeNames[CN_COUNT] = {
    [CN_VALUE] = "Value",
    [CN_INBUILD_CONST] = "Math-Constant",
    [CN_VARIABLE] = "Variable",
};

typedef union
{
  // TODO: Change to big number type!
  double value;
  e_math_constant_type inbuildConstant;
  // TODO: Rethink! Directly linking to the variable store could be easier than decoupling it with the name only.
  // node_variable_t *variable;
  string_slice_t variableName;
} u_const_node_as;

typedef struct
{
  e_const_node_type type;
  u_const_node_as as;
} node_const_t;

#define node_constant_value(val) \
  (node_const_t) { .type = CN_VALUE, .as.value = (val) }
#define node_constant_inbuild_const(val) \
  (node_const_t) { .type = CN_INBUILD_CONST, .as.inbuildConstant = (val) }
#define node_constant_variable(val) \
  (node_const_t) { .type = CN_VARIABLE, .as.variableName = (val) }

typedef union
{
  // TODO: Change to big number type!
  // double constant;
  node_const_t constant;
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

node_t *node_constant(arena_t *arena, size_t cursor, node_const_t constant)
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

// static bool check_semantics(lexer_t *lexer);
static node_t *try_parse_constant(arena_t *arena, lexer_t *lexer, size_t *index);
static node_t *try_parse_binop(arena_t *arena, lexer_t *lexer, size_t *index);
static node_t *try_parse_func(arena_t *arena, lexer_t *lexer, size_t *index);
static node_t *try_parse_paren(arena_t *arena, lexer_t *lexer, size_t *index);
static void print_node_ex(node_t *node, bool indented, size_t depth);
void print_node(node_t *node, bool indented);

typedef struct
{
  // Each indivually defined function argument name like "x,y,z", or "a1, a2, a3", ...
  // These are locally available in the given expression as variable symbols.
  // They will be replaced with the corresponding values when the function is called.
  string_slice_t *items;
  size_t capacity;
  size_t count;
} node_func_definition_args_t;

#define node_func_definition_args_append(a, funcArgs, arg) \
  arena_da_append(a, funcArgs, arg)

typedef struct
{
  // The name of the function.
  // With this name the function can be called in an expression.
  string_slice_t name;
  // A list of function argument names, which can be used, when using these symbols in the expression.
  node_func_definition_args_t args;
} node_function_definition_t;

typedef union
{
  string_slice_t varDef;
  node_function_definition_t funcDef;
} u_node_variable_as;

typedef enum
{
  // Just a constant variable definition. This means it evaluates directly down to a constant value like a math constant.
  NV_CONST_VARIABLE_DEFINITION,
  // A function definition. This means it will have at least one argument, which can be used in the assined expression.
  NV_FUNCTION_DEFINITION,

  NV_COUNT
} e_node_variable_type;

static_assert(NV_COUNT == 2, "Amount of variable-node-types have changed");

const char *nodeVariableTypeNames[NV_COUNT] = {
    [NV_CONST_VARIABLE_DEFINITION] = "Constant-Definition",
    [NV_FUNCTION_DEFINITION] = "Function-Definition",
};

typedef struct
{
  // The type of the variable definition.
  e_node_variable_type type;
  u_node_variable_as as;
  // The assigned expression tree to this variable
  node_t *expression;
} node_variable_t;

#define node_variable_constant(varName, value) ((node_variable_t){.type = NV_CONST_VARIABLE_DEFINITION, .as.varDef = (varName), .expression = (value)})
#define node_variable_function(varName, funcArgs, value) ((node_variable_t){.type = NV_FUNCTION_DEFINITION, .as.funcDef.name = (varName), .as.funcDef.args = (funcArgs), .expression = (value)})

// A list of all variables.
typedef struct
{
  node_variable_t *items;
  size_t capacity;
  size_t count;
} node_variable_stack_t;

#define node_variable_stack_append(a, stack, var) \
  arena_da_append(a, stack, var)

static bool try_parse_const_variable(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer);
static bool try_parse_func_variable(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer);
bool parse_node_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer);
void node_variable_stack_print(const node_variable_stack_t *varStack, bool showValue);

static node_t *parser_execute_ex(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer, size_t index);
node_t *parser_execute(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer);

node_t *ast_eval_ex(arena_t *arena, node_variable_stack_t *varStack, node_t *expr);

typedef struct
{
  // TODO: Change to big number!
  double value;
  bool isError;
} ast_eval_res_t;

ast_eval_res_t ast_eval(arena_t *arena, node_variable_stack_t *varStack, node_t *expr);

static bool try_parse_const_variable(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(varStack);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lexer->count <= 0)
    UNREACHABLE("No tokens found!");

  size_t index = 0;

  token_t *varNameTok = lex_at(lexer, index);

  if (!tok_is(varNameTok, TT_SYMBOL))
    return false;

  if (!lex_next_in_range(lexer, index))
    return false;

  token_t *assignTok = lex_at(lexer, index + 1);

  if (!tok_is_specific_literal(assignTok, CLT_EQUALS))
    return false;

  if (!lex_next_in_range(lexer, ++index))
  {
    S_ERROR(assignTok->cursor, "After a variable assignment must be an expression!");
    return false;
  }

  node_t *value = parser_execute_ex(varStack, arena, lexer, ++index);
  if (!value)
  {
    S_ERROR(assignTok->cursor, "After a variable assignment must be a valid expression!");
    return false;
  }

  // Create and add a new variable.
  node_variable_t constantVar = node_variable_constant(varNameTok->as.symbol, value);
  node_variable_stack_append(arena, varStack, constantVar);
  return true;
}

static bool try_parse_func_variable(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(varStack);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lexer->count <= 0)
    UNREACHABLE("No tokens found!");

  size_t index = 0;

  token_t *varNameTok = lex_at(lexer, index);

  if (!tok_is(varNameTok, TT_SYMBOL))
    return false;

  if (!lex_next_in_range(lexer, index))
    return false;

  token_t *oParenTok = lex_at(lexer, index + 1);

  if (!tok_is_paren(oParenTok, PT_OPAREN))
    return false;

  ++index;

  if (!lex_next_in_range(lexer, index))
  {
    S_ERROR(oParenTok->cursor, "Expected at least one function argument name and then a closing paren and an assigned expression!");
    return false;
  }

  node_func_definition_args_t funcArgs = {0};
  token_t *currTok = NULL;

  do
  {
    currTok = lex_at(lexer, ++index);

    if (tok_is(currTok, TT_SYMBOL))
      node_func_definition_args_append(arena, &funcArgs, currTok->as.symbol);
    else if (tok_is_specific_literal(currTok, CLT_COMMA))
      continue;
    else
      break;
  } while (lex_next_in_range(lexer, index));

  if (!lex_next_in_range(lexer, index))
  {
    S_ERROR(currTok->cursor, "Invalid function definition! Expected at least a closing paren, then an assignment and then an expression.");
    return false;
  }

  if (!tok_is_paren(currTok, PT_CPAREN))
  {
    S_ERROR(currTok->cursor, "Expected a closing paren, then an assignment and then an expression!");
    return false;
  }

  if (!lex_next_in_range(lexer, index))
  {
    S_ERROR(currTok->cursor, "Invalid function definition! Expected at least an assignment and then an expression.");
    return false;
  }

  token_t *assignTok = lex_at(lexer, index + 1);

  if (!tok_is_specific_literal(assignTok, CLT_EQUALS))
    return false;

  if (!lex_next_in_range(lexer, index))
  {
    S_ERROR(assignTok->cursor, "After a variable assignment must be an expression!");
    return false;
  }

  ++index;

  node_t *value = parser_execute_ex(varStack, arena, lexer, ++index);
  if (!value)
  {
    S_ERROR(assignTok->cursor, "After a variable assignment must be a valid expression");
    return false;
  }

  // Create and add a new variable.
  node_variable_t funcVar = node_variable_function(varNameTok->as.symbol, funcArgs, value);
  node_variable_stack_append(arena, varStack, funcVar);
  return true;
}

bool parse_node_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lexer->isError || lexer->count <= 0)
    return true;

  return try_parse_const_variable(vs, arena, lexer) ||
         try_parse_func_variable(vs, arena, lexer);
}

void node_variable_stack_print(const node_variable_stack_t *varStack, bool showValue)
{
  ASSERT_NULL(varStack);

  if (varStack->count <= 0)
  {
    printf("Variable-Stack is empty!\n");
    return;
  }

  printf("Printing variables (%zu):\n", varStack->count);

  for (size_t i = 0; i < varStack->count; ++i)
  {
    node_variable_t *variable = &varStack->items[i];

    switch (variable->type)
    {
    case NV_CONST_VARIABLE_DEFINITION:
      printf(SS_FMT, SS_ARG(&variable->as.varDef));
      break;
    case NV_FUNCTION_DEFINITION:
      printf(SS_FMT "(", SS_ARG(&variable->as.funcDef.name));
      for (size_t i = 0; i < variable->as.funcDef.args.count; ++i)
      {
        printf(SS_FMT, SS_ARG(&variable->as.funcDef.args.items[i]));
        if (i < variable->as.funcDef.args.count - 1)
          printf(", ");
      }
      printf(")");
      break;
    case NV_COUNT:
    default:
      UNREACHABLE("Invalid variable-type!");
      break;
    }

    if (!showValue)
      return;

    printf(" := ");
    print_node(variable->expression, false);
  }
}

// TODO: Rethink if needed! Can be checked while generating the AST with recursive descent parsing.
// static bool check_semantics(lexer_t *lexer)
// {
//   ASSERT_NULL(lexer);

//   if (lexer->count <= 0)
//     return false;

//   bool isError = false;
//   size_t parenCount = 0;

//   for (size_t i = 0; i < lexer->count; ++i)
//   {
//     token_t *tok = lex_at(lexer, i);

//     switch (tok->type)
//     {
//     // case TT_MATH_CONSTANT:
//     case TT_NUMBER:
//     {
//       if (i > 0)
//       {
//         token_t *lastTok = lex_at(lexer, i - 1);

//         if (tok_is(lastTok, TT_OPERATOR) ||
//             tok_is_specific_literal(lastTok, CLT_EQUALS) ||
//             tok_is_paren(lastTok, PT_OPAREN))
//           continue;

//         S_ERROR(tok->cursor, "Unexpected token before the number!");
//         isError = true;
//         continue;
//       }

//       // Valid number placement.
//       continue;
//     }
//     case TT_OPERATOR:
//     {
//       // Checks if this is the last token (the last must not be an opeartor).
//       if (!lex_next_in_range(lexer, i))
//       {
//         S_ERROR(tok->cursor, "An operator can't be the last token!");
//         isError = true;
//         continue;
//       }

//       token_t *nextTok = lex_at(lexer, i + 1);

//       if (i > 0)
//       {
//         token_t *lastTok = lex_at(lexer, i - 1);

//         // Checks if the last token was a number or a closing paren.
//         if (tok_is(lastTok, TT_NUMBER) ||
//             // TOOD: Rethink! Maybe change to TT_SYMBOL and check if it is a math constant in the parser.
//             // tok_is(lastTok, TT_MATH_CONSTANT) ||
//             tok_is(lastTok, TT_SYMBOL) ||
//             tok_is_paren(lastTok, PT_CPAREN))
//           continue;

//         // Checks for ".. ')' OPERATOR NUMBER ..".
//         if (tok_is_paren(lastTok, PT_CPAREN) &&
//             tok_is(nextTok, TT_NUMBER))
//           continue;

//         // Checks for ".. '(' '+'/'-' NUMBER ..".
//         if (tok_is_paren(lastTok, PT_OPAREN) &&
//             tok_is_number_operator(tok) &&
//             tok_is(nextTok, TT_NUMBER))
//           continue;

//         // Checks if last token was an operator, the current is a number operator and the next is a number ".. OPERATOR '+'/'-' NUMBER ..".
//         if (tok_is(lastTok, TT_OPERATOR) &&
//             tok_is_number_operator(tok) &&
//             tok_is(nextTok, TT_NUMBER))
//           continue;
//       }

//       if (tok_is_number_operator(tok) && tok_is(nextTok, TT_NUMBER))
//         continue;

//       S_ERROR(tok->cursor, "Invalid usage of an operator!");
//       isError = true;
//       continue;
//     }
//     case TT_PAREN:
//     {
//       e_paren_type ptype = tok->as.paren;

//       if (ptype == PT_OPAREN)
//       {
//         parenCount++;

//         if (i > 0)
//         {
//           token_t *lastTok = lex_at(lexer, i - 1);

//           if (tok_is_paren(lastTok, PT_CPAREN))
//           {
//             S_ERROR(tok->cursor, "Expected operator! Before an open paren must NOT be a closing paren.");
//             isError = true;
//             continue;
//           }

//           if (tok_not(lastTok, TT_OPERATOR) && !tok_is_paren(lastTok, PT_OPAREN))
//           {
//             S_ERROR(tok->cursor, "Expected operator or open paren!");
//             isError = true;
//             continue;
//           }
//         }
//       }
//       else if (ptype == PT_CPAREN)
//       {
//         if (parenCount <= 0)
//         {
//           S_ERROR(tok->cursor, "Too many closing parens!");
//           isError = true;
//           continue;
//         }

//         parenCount--;

//         if (i > 0)
//         {
//           token_t *lastTok = lex_at(lexer, i - 1);

//           if (tok_is(lastTok, TT_OPERATOR))
//           {
//             S_ERROR(tok->cursor, "Expected an expression after an operator but got a closing paren!");
//             isError = true;
//             continue;
//           }

//           if (tok_is_paren(lastTok, PT_OPAREN))
//           {
//             S_ERROR(tok->cursor, "Expected an argument expression inside the parens!");
//             isError = true;
//             continue;
//           }
//         }
//       }

//       if (!lex_next_in_range(lexer, i) && parenCount > 0)
//       {
//         S_ERROR(tok->cursor, "Expected closing paren!");
//         isError = true;
//         continue;
//       }

//       continue;
//     }
//     // TODO: Rethink! Maybe change to TT_SYMBOL and check if it is a function in the parser.
//     // case TT_FUNCTION:
//     // {
//     //   // Checks that after the current initializer there are still at least 3 more tokens because a function needs
//     //   // an open paren, at least a single argument and a closing paren.
//     //   if (!lex_in_range(lexer, i + 2))
//     //   {
//     //     S_ERROR(tok->cursor, "A function initializer can't be the last token because it needs an open and a closing paren and an argument expression inside them!");
//     //     isError = true;
//     //     continue;
//     //   }

//     //   token_t *nextTok = lex_at(lexer, i + 1);

//     //   // Checks if last token was a function initializer and also if the current is an open paren ("FUNC(<-...)").
//     //   if (tok_not_specific_paren(nextTok, PT_OPAREN))
//     //   {
//     //     S_ERROR(nextTok->cursor, "Expected an open paren after a function initializer!");
//     //     isError = true;
//     //     continue;
//     //   }

//     //   if (i > 0)
//     //   {
//     //     token_t *lastTok = lex_at(lexer, i - 1);

//     //     // Checks if the last token was an operator or an open paren.
//     //     if (tok_not(lastTok, TT_OPERATOR) &&
//     //         tok_not_specific_paren(lastTok, PT_OPAREN))
//     //     {
//     //       S_ERROR(lastTok->cursor, "Before a function initializer must be an operator or an open paren!");
//     //       isError = true;
//     //       continue;
//     //     }
//     //   }

//     //   // This ignores the cheking for the next token because the checks here make sure that the next is an open paren.
//     //   parenCount++;
//     //   i++;

//     //   continue;
//     // }
//     case TT_LITERAL:
//     {
//       // TODO: Implement!
//       // Current literals:
//       // > ',': for multi argument functions like '... funcTest(arg1, arg2, arg3) ...'.
//       // > '=': for equations like '10 + 5 = 20 - 5'. This could return f.e. 'true' or 'false'.
//       //        Also it could be used for assigning an expression to a variable or creating a new function.
//       //        Like:
//       //            'test = 10 + 5'
//       //            'test(x, y) = 20 * 5 - sqrt(x) ^ y'

//       switch (tok->as.literal)
//       {
//       case CLT_COMMA:
//       {
//         if (i <= 0)
//         {
//           S_ERROR(tok->cursor, "This literal can't be at the start!");
//           isError = true;
//           continue;
//         }

//         token_t *lastTok = lex_at(lexer, i - 1);

//         if (tok_is(lastTok, TT_LITERAL) ||
//             // TODO: Rethink! Maybe change to TT_SYMBOL and check if it is a function in the parser.
//             // tok_is(lastTok, TT_FUNCTION) ||
//             tok_is_paren(lastTok, PT_OPAREN) ||
//             tok_is(lastTok, TT_OPERATOR) ||
//             tok_is(lastTok, TT_SYMBOL))
//         {
//           S_ERROR(tok->cursor, "Before this literal there must be a valid argument!");
//           isError = true;
//           continue;
//         }

//         if (!lex_next_in_range(lexer, i))
//         {
//           S_ERROR(tok->cursor, "This literal must have a valid argument after it!");
//           isError = true;
//           continue;
//         }

//         token_t *nextTok = lex_at(lexer, i + 1);

//         if (tok_is(nextTok, TT_LITERAL) ||
//             tok_is(nextTok, TT_SYMBOL) ||
//             tok_is_paren(nextTok, PT_CPAREN) ||
//             tok_is(nextTok, TT_OPERATOR))
//         {
//           S_ERROR(tok->cursor, "After this literal must be a valid argument!");
//           isError = true;
//           continue;
//         }

//         // TODO: Implement the checks to make sure this is only used inside a function definition!

//         break;
//       }
//       case CLT_EQUALS:
//       {
//         if (i <= 0)
//         {
//           S_ERROR(tok->cursor, "A variable assignment literal can't be at the start!");
//           isError = true;
//           continue;
//         }

//         token_t *lastTok = lex_at(lexer, i - 1);

//         // Checks if the last token was an operator or an open paren.
//         if (tok_not(lastTok, TT_SYMBOL))
//         {
//           S_ERROR(lastTok->cursor, "Before a variable assignment literal must be a variable name symbol!");
//           isError = true;
//           continue;
//         }

//         continue;
//       }
//       case CLT_COUNT:
//       case CLT_INVALID:
//       default:
//         UNREACHABLE("Unreachable literal type!");
//       }

//       S_ERROR(tok->cursor, "Literal not implemented yet!");
//       isError = true;
//       continue;
//     }
//     case TT_SYMBOL:
//     {
//       if (i != 0)
//       {
//         S_ERROR(tok->cursor, "Variable name definition must be at the start!");
//         isError = true;
//         continue;
//       }

//       // Checks that after the current initializer there are still at least 2 more tokens because a variable definition needs
//       // an variable definition literal and at least a single argument.
//       if (!lex_in_range(lexer, i + 2))
//       {
//         S_ERROR(tok->cursor, "A variable name definition can't be the last token because it needs a variable assignment literal and at least a single argument!");
//         isError = true;
//         continue;
//       }

//       token_t *nextTok = lex_at(lexer, i + 1);

//       if (tok_not_specific_literal(nextTok, CLT_EQUALS))
//       {
//         S_ERROR(nextTok->cursor, "Expected a variable assignment literal after variable name definition!");
//         isError = true;
//         continue;
//       }

//       ++i;

//       nextTok = lex_at(lexer, i + 1);

//       // Valid variable name definition.
//       if (tok_is(nextTok, TT_NUMBER) ||
//           // TODO: Rethink! Maybe change to TT_SYMBOL and check if it is a math constant in the parser.
//           // tok_is(nextTok, TT_MATH_CONSTANT) ||
//           // TODO: Rethink! Maybe change to TT_SYMBOL and check if it is a function in the parser.
//           // tok_is(nextTok, TT_FUNCTION) ||
//           tok_is_paren(nextTok, PT_OPAREN) ||
//           tok_is(nextTok, TT_SYMBOL) ||
//           (lex_in_range(lexer, i + 2) && tok_is_paren(lex_at(lexer, i + 2), PT_OPAREN)))
//         continue;

//       S_ERROR(nextTok->cursor, "Expected a valid argument after variable assignment literal!");
//       isError = true;
//       continue;
//     }
//     // TODO: Rethink!
//     case TT_EOL:
//     case TT_EOF:
//     case TT_COUNT:
//     default:
//       UNREACHABLE("Invalid token-type!");
//     }
//   }

//   if (parenCount != 0 && !isError)
//   {
//     S_ERROR(lex_at(lexer, lexer->count - 1)->cursor, "Invalid paren usage!");
//     isError = true;
//   }

//   return isError;
// }

static node_t *try_parse_constant(arena_t *arena, lexer_t *lexer, size_t *index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t *token = lex_at(lexer, *index);

  if (tok_is(token, TT_NUMBER))
  {
    node_const_t constant = node_constant_value(token->as.number);
    return node_constant(arena, token->cursor, constant);
  }
  else if (tok_is(token, TT_SYMBOL) && cstr_is_math_constant_ex(token->as.symbol.src, token->as.symbol.len))
  {
    // Checks if the symbol is a valid inbuild math constant name.
    e_math_constant_type mc = cstr_to_math_constant_type_ex(token->as.symbol.src, token->as.symbol.len);
    if (mc == MC_INVALID)
      UNREACHABLE("Math-Constant-Type not implemented!");

    node_const_t constant = node_constant_inbuild_const(mc);
    return node_constant(arena, token->cursor, constant);
  }
  else if (tok_is(token, TT_SYMBOL))
  {
    // TODO: Maybe try to parse variables here too!
    // 1. Maybe try to eval the complete variable down to a constant.
    // 2. Maybe try to store the complete node tree inside the constant as a reference and mark it so that it is possible to differantiate.
    // 3. Store the variable name / variable arg reference to cross check later.

    node_const_t constant = node_constant_variable(token->as.symbol);
    return node_constant(arena, token->cursor, constant);
  }

  return NULL;
}

static node_t *try_parse_binop(arena_t *arena, lexer_t *lexer, size_t *index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t *token = lex_at(lexer, *index);

  node_t *lhs = try_parse_constant(arena, lexer, index);
  if (!lhs)
  {
    S_ERROR(token->cursor, "Expected a constant value!");
    return NULL;
  }

  // If nothing is left, return the constant
  if (!lex_next_in_range(lexer, *index))
    return lhs;

  token_t *opTok = lex_at(lexer, ++(*index));

  if (!tok_is(opTok, TT_OPERATOR))
  {
    S_ERROR(opTok->cursor, "Expected an operator!");
    return NULL;
  }

  ++(*index);

  node_t *rhs = try_parse_binop(arena, lexer, index);
  if (!rhs)
    return NULL;

  return node_binop(arena, opTok->cursor, to_local_binop_type(opTok->as.operator), lhs, rhs);
}

static node_t *try_parse_func(arena_t *arena, lexer_t *lexer, size_t *index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t *token = lex_at(lexer, *index);

  // TODO: Rethink! Maybe change to TT_SYMBOL and check if it is a function in the parser.
  // if (tok_is(token, TT_FUNCTION))
  if (tok_is(token, TT_SYMBOL))
  {
    // Checks if the symbol is a valid function name.
    e_function_type ft = cstr_to_function_type_ex(token->as.symbol.src, token->as.symbol.len);
    if (ft == FT_INVALID)
      return NULL;

    // Next token must be an open paren.
    if (!lex_next_in_range(lexer, *index))
      return NULL;

    token_t *nextTok = lex_at(lexer, *index + 1);

    if (!tok_is_paren(nextTok, PT_OPAREN))
      return NULL;

    // Next token must be an argument of the function. This can be a number, math constant, function, or an open paren.
    // Can also be multiple arguments separated by a COMMA.
    // Make sure that all function arguments get loaded. NOT JUST ONE!
    // node_func_args_t argsList = {0};

    // node_t *argX = node_constant(&arena, ...);
    // node_t ...
    // node_func_args_append(arena, &argsList, argx);

    // Next token must be a closing paren.

    // return node_func(arena, token->cursor, to_local_func_type(token->as.function), argsList);
    return NULL;
  }

  return NULL;
}

static node_t *try_parse_paren(arena_t *arena, lexer_t *lexer, size_t *index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t *token = lex_at(lexer, *index);

  if (tok_is(token, TT_PAREN))
  {
    // TODO: Implement! Parse everything till the corresponding closing paren.
    // Paren-Arguments can contain sub parens, functions, etc.
    return NULL;
  }

  return NULL;
}

static node_t *parser_execute_ex(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer, size_t index)
{
  // TODO:
  // Order checking: Brackets -> Exponents -> Multiplication / Division -> Addition / Substraction
  // A stack for checking nested expressions in brackets. Also if the brackets are used correctly.
  // Paren checking: 2 Open '(' should have 2 Closing ')' after.
  // After a constant / number MUST come an operator, function, or closing paren

  // IN: "EN + 5 / (5 * 0)"
  // AST: "add(EN, divide(5, paren(mult(5, 0))))"

  ASSERT_NULL(varStack);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lexer->isError || lexer->count <= 0)
    return NULL;

  if (index > lexer->count - 1)
    UNREACHABLE("Lexer out of range!");

  // TODO: Rethink!
  if (tok_is(lex_at(lexer, index), TT_EOL))
    return NULL;

  if (tok_is(lex_at(lexer, index), TT_EOF))
    return NULL;

  node_t *nodeBinOp = try_parse_binop(arena, lexer, &index);
  if (nodeBinOp)
    return nodeBinOp;

  node_t *nodeConst = try_parse_constant(arena, lexer, &index);
  if (nodeConst)
    return nodeConst;

  return NULL;
}

node_t *parser_execute(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(varStack);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lexer->isError || lexer->count <= 0)
    return NULL;

  fprintf(stderr, "WARNING: Parser is currently under developement!\n");

  // TODO: Rethink!
  // if (!check_semantics(lexer))
  //   return NULL;

  return parser_execute_ex(varStack, arena, lexer, 0);
}

double node_constant_to_value(arena_t *arena, node_variable_stack_t *varStack, node_t *node)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(varStack);
  ASSERT_NULL(node);

  if (node->type != NT_CONSTANT)
    UNREACHABLE("Invalid node type!");

  node_const_t *constant = &node->as.constant;

  switch (constant->type)
  {
  case CN_VALUE:
    return constant->as.value;
  case CN_INBUILD_CONST:
  {
    e_math_constant_type mc = constant->as.inbuildConstant;
    if (mc == MC_INVALID)
      UNREACHABLE("Invalid math constant type!");
    return mathConstantTypeValues[mc];
  }
  break;
  case CN_VARIABLE:
  {
    string_slice_t *varName = &constant->as.variableName;
    for (size_t i = 0; i < varStack->count; ++i)
    {
      node_variable_t *currVar = &varStack->items[i];
      // TODO: Rethink! Only constant variable parsing.
      if (currVar->type != NV_CONST_VARIABLE_DEFINITION)
        continue;
      string_slice_t *currVarName = &currVar->as.varDef;
      if (currVarName->len != varName->len)
        continue;
      if (strncmp((char *)currVarName->current_ptr, (char *)varName->current_ptr, (size_t)(currVarName->current_ptr - currVarName->start_ptr)))
      {
        // TODO: Rethink and test!
        ast_eval_res_t res = ast_eval(arena, varStack, currVar->expression);
        if (res.isError)
          return 0;

        return res.value;
      }
    }

    // TODO: Rethink!
    E_ERROR(node->cursor, "Variable not defined!");
    return 0;
  }
  break;
  case CN_COUNT:
  default:
    UNREACHABLE("Invalid constant node type!");
  }
}

// TODO: Rethink and test!
node_t *ast_eval_ex(arena_t *arena, node_variable_stack_t *varStack, node_t *expr)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(varStack);
  ASSERT_NULL(expr);

  switch (expr->type)
  {
  case NT_CONSTANT:
  {
    // Will evaluate down to a constant value type.
    if (expr->as.constant.type == CN_VALUE)
      return expr;
    node_const_t constant = node_constant_value(node_constant_to_value(arena, varStack, expr));
    return node_constant(arena, expr->cursor, constant);
  }
  case NT_BINOP:
  {
    node_t *lhs = ast_eval_ex(arena, varStack, expr->as.binop.lhs);
    if (!lhs)
      return NULL;

    node_t *rhs = ast_eval_ex(arena, varStack, expr->as.binop.rhs);
    if (!rhs)
      return NULL;

    switch (expr->as.binop.type)
    {
    case NO_ADD:
    {
      // return node_constant(arena, expr->cursor, lhs->as.constant + rhs->as.constant);
      node_const_t constant = node_constant_value(
          node_constant_to_value(arena, varStack, lhs) + node_constant_to_value(arena, varStack, rhs));
      return node_constant(arena, expr->cursor, constant);
    }
    case NO_SUB:
    {
      // return node_constant(arena, expr->cursor, lhs->as.constant - rhs->as.constant);
      node_const_t constant = node_constant_value(
          node_constant_to_value(arena, varStack, lhs) - node_constant_to_value(arena, varStack, rhs));
      return node_constant(arena, expr->cursor, constant);
    }
    case NO_MUL:
    {
      // return node_constant(arena, expr->cursor, lhs->as.constant * rhs->as.constant);
      node_const_t constant = node_constant_value(
          node_constant_to_value(arena, varStack, lhs) * node_constant_to_value(arena, varStack, rhs));
      return node_constant(arena, expr->cursor, constant);
    }
    case NO_DIV:
    {
      // if (rhs->as.constant == 0)
      if (node_constant_to_value(arena, varStack, rhs) == 0)
      {
        E_ERROR(rhs->cursor, "Tried to divide by zero!");
        return NULL;
      }
      // return node_constant(arena, expr->cursor, lhs->as.constant / rhs->as.constant);
      node_const_t constant = node_constant_value(
          node_constant_to_value(arena, varStack, lhs) / node_constant_to_value(arena, varStack, rhs));
      return node_constant(arena, expr->cursor, constant);
    }
    case NO_POW:
    {
      // return node_constant(arena, expr->cursor, pow(lhs->as.constant, rhs->as.constant));
      node_const_t constant = node_constant_value(
          pow(node_constant_to_value(arena, varStack, lhs), node_constant_to_value(arena, varStack, rhs)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NO_COUNT:
    default:
      UNREACHABLE("Invalid binop-node-type!");
    }
    break;
  }
  case NT_FUNCTION:
  {
    // TODO: Check the argument count with the "functionTypeParameterCount" list!
    switch (expr->as.func.type)
    {
    case NF_SQRT:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "sqrt() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, arg->cursor, sqrt(arg->as.constant));
      node_const_t constant = node_constant_value(sqrt(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_EXP:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "exp() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, arg->cursor, exp(arg->as.constant));
      node_const_t constant = node_constant_value(exp(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_SIN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "sin() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, arg->cursor, sin(arg->as.constant));
      node_const_t constant = node_constant_value(sin(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_ASIN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "asin() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, arg->cursor, asin(arg->as.constant));
      node_const_t constant = node_constant_value(asin(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_SINH:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "sinh() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, arg->cursor, sinh(arg->as.constant));
      node_const_t constant = node_constant_value(sinh(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_COS:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "cos() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, func->cursor, cos(func->as.constant));
      node_const_t constant = node_constant_value(cos(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_ACOS:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "acos() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, arg->cursor, acos(arg->as.constant));
      node_const_t constant = node_constant_value(acos(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_COSH:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "cosh() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, arg->cursor, cosh(arg->as.constant));
      node_const_t constant = node_constant_value(cosh(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_TAN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "tanh() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, arg->cursor, tan(arg->as.constant));
      node_const_t constant = node_constant_value(tan(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_ATAN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "atan() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, arg->cursor, atan(arg->as.constant));
      node_const_t constant = node_constant_value(atan(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_TANH:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "tanh() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, arg->cursor, tanh(arg->as.constant));
      node_const_t constant = node_constant_value(tanh(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_ATAN2:
    {
      if (expr->as.func.args.count != 2)
      {
        E_ERROR(expr->cursor, "atan2() takes two arguments!");
        return NULL;
      }
      node_t *arg1 = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg1)
        return NULL;
      node_t *arg2 = ast_eval_ex(arena, varStack, expr->as.func.args.items[1]);
      if (!arg2)
        return NULL;
      // return node_constant(arena, expr->cursor, atan2(arg1->as.constant, arg2->as.constant));
      node_const_t constant = node_constant_value(atan2(node_constant_to_value(arena, varStack, arg1), node_constant_to_value(arena, varStack, arg2)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_LN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "ln() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, arg->cursor, log(arg->as.constant));
      node_const_t constant = node_constant_value(log(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_LOG10:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "log10() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, varStack, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      // return node_constant(arena, arg->cursor, log10(arg->as.constant));
      node_const_t constant = node_constant_value(log10(node_constant_to_value(arena, varStack, arg)));
      return node_constant(arena, expr->cursor, constant);
    }
    case NF_COUNT:
    default:
      UNREACHABLE("Invalid function-node-type!");
    }
    break;
  }
  case NT_PAREN:
  {
    node_t *arg = ast_eval_ex(arena, varStack, expr->as.paren.arg);
    if (!arg)
      return NULL;
    return node_constant(arena, expr->cursor, arg->as.constant);
  }
  case NT_COUNT:
  default:
    UNREACHABLE("Invalid node-type!");
  }
}

ast_eval_res_t ast_eval(arena_t *arena, node_variable_stack_t *varStack, node_t *expr)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(varStack);
  ASSERT_NULL(expr);

  ast_eval_res_t result = {0};

  node_t *evaluatedNode = ast_eval_ex(arena, varStack, expr);
  if (!evaluatedNode)
  {
    fprintf(stderr, "ERROR: Evaluation failed!\n");
    result.isError = true;
    return result;
  }

  if (evaluatedNode->type != NT_CONSTANT)
    UNREACHABLE("Evaluation failed! Return-Type not of type 'CONSTANT'.");

  if (evaluatedNode->as.constant.type != CN_VALUE)
    UNREACHABLE("Evaluation failed! Return-Type of constant not of type 'VALUE'.");

  result.value = evaluatedNode->as.constant.as.value;
  return result;
}

#define _PRINT_DEPTH_SPACES(indented, depth) \
  do                                         \
  {                                          \
    if (indented)                            \
      for (size_t i = 0; i < (depth); ++i)   \
        printf(" ");                         \
  } while (0)

static void print_node_ex(node_t *node, bool indented, size_t depth)
{
  ASSERT_NULL(node);

  switch (node->type)
  {
  case NT_CONSTANT:
  {
    _PRINT_DEPTH_SPACES(indented, depth);
    switch (node->as.constant.type)
    {
    case CN_VALUE:
    {
      printf(DOUBLE_PRINT_FORMAT, node->as.constant.as.value);
    }
    break;
    case CN_INBUILD_CONST:
    {
      e_math_constant_type mc = node->as.constant.as.inbuildConstant;
      if (mc == MC_INVALID)
        UNREACHABLE("Invalid math constant type!");

      printf("%s (%s = " DOUBLE_PRINT_FORMAT ")", mathConstantTypeIdentifiers[mc], mathConstantTypeNames[mc], mathConstantTypeValues[mc]);
    }
    break;
    case CN_VARIABLE:
    {
      // TODO: Rethink! Maybe link directly to the variable expression or just show the variable name here.
      printf(SS_FMT, SS_ARG(&node->as.constant.as.variableName));
    }
    break;
    case CN_COUNT:
    default:
      UNREACHABLE("Invalid constant-node-type!");
    }
    break;
  }
  case NT_BINOP:
  {
    if (node->as.binop.type >= NO_COUNT)
      UNREACHABLE("Invalid binop-node-type!");

    _PRINT_DEPTH_SPACES(indented, depth);
    printf("%s(", nodeBinopTypeNames[node->as.binop.type]);
    if (indented)
      printf("\n");
    print_node_ex(node->as.binop.lhs, indented, depth + 1);
    printf(",%s", indented ? "\n" : " ");
    print_node_ex(node->as.binop.rhs, indented, depth + 1);
    if (indented)
      printf("\n");
    _PRINT_DEPTH_SPACES(indented, depth);
    printf(")");
    break;
  }
  case NT_FUNCTION:
  {
    node_function_t *funcNode = &node->as.func;
    if (funcNode->type >= NF_COUNT)
      UNREACHABLE("Invalid func-node-type!");

    node_func_args_t *args = &funcNode->args;

    if (args->count <= 0)
      UNREACHABLE("Function node has no arguments!");

    _PRINT_DEPTH_SPACES(indented, depth);
    printf("%s(", nodeFunctionTypeNames[funcNode->type]);
    if (args->count == 1)
    {
      bool isArgTypeConst = args->items[0]->type == NT_CONSTANT;

      if (indented && !isArgTypeConst)
        printf("\n");

      print_node_ex(args->items[0], indented, !isArgTypeConst ? depth + 1 : 0);

      if (indented && !isArgTypeConst)
        printf("\n");

      _PRINT_DEPTH_SPACES(indented, !isArgTypeConst ? depth : 0);
    }
    else
    {
      if (indented)
        printf("\n");
      for (size_t i = 0; i < args->count; ++i)
      {
        print_node_ex(args->items[i], indented, depth + 1);
        if (i + 1 < args->count)
          printf(",%s", indented ? "\n" : " ");
      }
      if (indented)
        printf("\n");
      _PRINT_DEPTH_SPACES(indented, depth);
    }
    printf(")");
    break;
  }
  case NT_PAREN:
  {
    _PRINT_DEPTH_SPACES(indented, depth);
    printf("paren(");
    bool isArgTypeConst = node->as.paren.arg->type == NT_CONSTANT;
    if (indented && !isArgTypeConst)
      printf("\n");
    print_node_ex(node->as.paren.arg, indented, !isArgTypeConst ? depth + 1 : 0);
    if (indented && !isArgTypeConst)
      printf("\n");
    _PRINT_DEPTH_SPACES(indented, !isArgTypeConst ? depth : 0);
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

bool execute_test(arena_t *arena, node_variable_stack_t *varStack, node_test_t *test, bool freeArena, bool verbose, bool indented)
{
  if (verbose)
  {
    printf("\n");
    print_node(test->expressionTree, indented);
    printf("\n");
  }

  printf("Input       = %s\n", test->inputExpression);
  if (!test->shouldFail)
    printf("Expected    = " DOUBLE_PRINT_FORMAT "\n", test->expectedResult);

  ast_eval_res_t evalResult = ast_eval(arena, varStack, test->expressionTree);

  if (test->shouldFail)
  {
    if (!evalResult.isError)
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
    if (evalResult.isError)
    {
      fprintf(stderr, "ERROR: Evaluation failed!\n");

      if (freeArena)
        arena_free(arena);

      return false;
    }

    if (freeArena)
      arena_free(arena);

    printf("Result      = " DOUBLE_PRINT_FORMAT "\n", evalResult.value);

    if (!compare_doubles(evalResult.value, test->expectedResult, 5))
    {
      fprintf(stderr, "ERROR: Evaluated result not equal to expected value!\n");
      return false;
    }
  }

  return true;
}

#endif // _PARSER_H_
