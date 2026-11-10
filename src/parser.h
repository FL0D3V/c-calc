#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"

#define _G_ERROR_NAME "ERROR"
#define _S_ERROR_NAME "SEMANTIC-ERROR"
#define _E_ERROR_NAME "EVALUATION-ERROR"
#define _PARSER_ERROR(type, cursor, message) \
  fprintf(stderr, type ":%zu: %s\n", cursor, message)
#define _PARSER_ERROR_SS(type, cursor, message, ss) \
  fprintf(stderr, type ":%zu: " message "\n", cursor, SS_ARG(ss))
#define G_ERROR(message) \
  fprintf(stderr, _G_ERROR_NAME ": %s\n", message)
#define S_ERROR(cursor, message) _PARSER_ERROR(_S_ERROR_NAME, cursor, message)
#define S_ERROR_SS(cursor, message, ss) _PARSER_ERROR_SS(_S_ERROR_NAME, cursor, message, ss)
#define E_ERROR(cursor, message) _PARSER_ERROR(_E_ERROR_NAME, cursor, message)
#define E_ERROR_SS(cursor, message, ss) _PARSER_ERROR_SS(_E_ERROR_NAME, cursor, message, ss)

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

typedef enum
{
  CN_VALUE,
  CN_BUILDIN_CONST,
  CN_VARIABLE,

  CN_COUNT
} e_const_node_type;

static_assert(CN_COUNT == 3, "Amount of value node types have changed");

const char *constNodeTypeNames[CN_COUNT] = {
    [CN_VALUE] = "Value",
    [CN_BUILDIN_CONST] = "Math-Constant",
    [CN_VARIABLE] = "Variable",
};

typedef union
{
  // TODO: Change to big number type!
  double value;
  e_math_constant_type buildinConstant;
  string_slice_t variableName;
} u_const_node_as;

typedef struct
{
  e_const_node_type type;
  u_const_node_as as;
  bool hasPreMinus;
} node_const_t;

#define node_constant_value_ex(val, minus) \
  (node_const_t) { .type = CN_VALUE, .as.value = (val), .hasPreMinus = (minus) }
#define node_constant_value(val) \
  (node_const_t) { .type = CN_VALUE, .as.value = (val) }
#define node_constant_buildin_const_ex(val, minus) \
  (node_const_t) { .type = CN_BUILDIN_CONST, .as.buildinConstant = (val), .hasPreMinus = (minus) }
#define node_constant_buildin_const(val) node_constant_buildin_const_ex(val, false)
#define node_constant_variable_ex(val, minus) \
  (node_const_t) { .type = CN_VARIABLE, .as.variableName = (val), .hasPreMinus = (minus) }
#define node_constant_variable(val) node_constant_variable_ex(val, false)

typedef union
{
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
  node_t *newNode = (node_t *)arena_alloc(arena, sizeof(node_t));
  *newNode = (node_t){
      .type = type,
      .cursor = cursor,
  };
  // TODO:
  // newNode->type = type;
  // newNode->cursor = cursor;
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
  e_node_variable_type type;
  u_node_variable_as as;
  // The assigned expression tree to this variable
  node_t *expression;
} node_variable_t;

#define node_variable_constant(varName, value) ((node_variable_t){.type = NV_CONST_VARIABLE_DEFINITION, .as.varDef = (varName), .expression = (value)})
#define node_variable_function(varName, funcArgs, value) ((node_variable_t){.type = NV_FUNCTION_DEFINITION, .as.funcDef.name = (varName), .as.funcDef.args = (funcArgs), .expression = (value)})

typedef struct
{
  node_variable_t *items;
  size_t capacity;
  size_t count;
} node_variable_stack_t;

typedef enum
{
  FVM_ALL,
  FVM_CONST,
  FVM_FUNC,

  FVM_COUNT
} e_find_var_mode;

static_assert(FVM_COUNT == 3, "Amount of find node var modes have changed!");

bool vs_name_exists_ex(node_variable_stack_t *vs, string_slice_t *name, e_find_var_mode mode)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(name);

  for (size_t i = 0; i < vs->count; ++i)
  {
    node_variable_t *curr = &vs->items[i];
    string_slice_t *currName = NULL;

    switch (mode)
    {
    case FVM_ALL:
    {
      switch (curr->type)
      {
      case NV_CONST_VARIABLE_DEFINITION:
      {
        currName = &curr->as.varDef;
      }
      break;
      case NV_FUNCTION_DEFINITION:
      {
        currName = &curr->as.funcDef.name;
      }
      break;
      case NV_COUNT:
      default:
        UNREACHABLE("Invalid node variable type!");
      }
    }
    break;
    case FVM_CONST:
    {
      if (curr->type == NV_CONST_VARIABLE_DEFINITION)
        currName = &curr->as.varDef;
    }
    break;
    case FVM_FUNC:
    {
      if (curr->type == NV_FUNCTION_DEFINITION)
        currName = &curr->as.funcDef.name;
    }
    break;
    case FVM_COUNT:
    default:
      UNREACHABLE("Invalid find var mode!");
    }

    if (!currName)
      continue;

    if (ss_compare(name, currName))
      return true;
  }
  return false;
}

#define vs_name_exists(vs, name) vs_name_exists_ex((vs), (name), FVM_ALL)

node_variable_t *find_node_variable_ex(node_variable_stack_t *vs, string_slice_t *name, e_find_var_mode mode)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(name);

  for (size_t i = 0; i < vs->count; ++i)
  {
    node_variable_t *curr = &vs->items[i];
    string_slice_t *currName = NULL;

    switch (mode)
    {
    case FVM_ALL:
    {
      switch (curr->type)
      {
      case NV_CONST_VARIABLE_DEFINITION:
      {
        currName = &curr->as.varDef;
      }
      break;
      case NV_FUNCTION_DEFINITION:
      {
        currName = &curr->as.funcDef.name;
      }
      break;
      case NV_COUNT:
      default:
        UNREACHABLE("Invalid node variable type!");
      }
    }
    break;
    case FVM_CONST:
    {
      if (curr->type == NV_CONST_VARIABLE_DEFINITION)
        currName = &curr->as.varDef;
    }
    break;
    case FVM_FUNC:
    {
      if (curr->type == NV_FUNCTION_DEFINITION)
        currName = &curr->as.funcDef.name;
    }
    break;
    case FVM_COUNT:
    default:
      UNREACHABLE("Invalid find var mode!");
    }

    if (!currName)
      continue;

    // Check if the given name is in the var stack.
    if (ss_compare(name, currName))
      return curr;
  }

  return NULL;
}

#define find_node_variable(vs, name) find_node_variable_ex((vs), (name), FVM_ALL)

#define node_variable_stack_append(a, stack, var) \
  arena_da_append(a, stack, var)

static bool try_parse_const_variable(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer);
static bool try_parse_func_variable(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer);
bool parse_node_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer);
void node_variable_stack_print(node_variable_stack_t *varStack, bool showValue);

node_t *parser_execute(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer, size_t *index);
static void print_node_ex(node_variable_stack_t *vs, node_t *node, bool indented, size_t depth);
void print_node(node_variable_stack_t *vs, node_t *node, bool indented);

node_t *ast_eval_ex(arena_t *arena, node_variable_stack_t *varStack, node_t *expr);

typedef struct
{
  // TODO: Change to big number!
  double value;
  bool isError;
} ast_eval_res_t;

ast_eval_res_t ast_eval(arena_t *arena, node_variable_stack_t *varStack, node_t *expr);

static bool check_variable_name_errors(node_variable_stack_t *vs, string_slice_t *name, size_t cursor)
{
  if (vs_name_exists(vs, name))
  {
    S_ERROR_SS(cursor, "Variable '" SS_FMT "' already exists!", name);
    return false;
  }
  if (ss_in_math_constants(name))
  {
    S_ERROR_SS(cursor, "Variable name '" SS_FMT "' overlaps with a buildin math constant!", name);
    return false;
  }
  if (ss_in_function_types(name))
  {
    S_ERROR_SS(cursor, "Variable name '" SS_FMT "' overlaps with a buildin function!", name);
    return false;
  }
  return true;
}

static bool try_parse_const_variable(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(varStack);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lexer->count <= 0)
    return true;

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

  ++index;
  node_t *value = parser_execute(varStack, arena, lexer, &index);
  if (!value)
  {
    S_ERROR(assignTok->cursor, "After a variable assignment must be a valid expression!");
    return false;
  }

  if (!check_variable_name_errors(varStack, &varNameTok->as.symbol, varNameTok->cursor))
    return false;

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
    return true;

  size_t index = 0;

  token_t *varNameTok = lex_at(lexer, index);

  if (!tok_is(varNameTok, TT_SYMBOL))
    return false;

  if (!lex_next_in_range(lexer, index))
    return false;

  token_t *oParenTok = lex_at(lexer, index + 1);

  if (!tok_is_paren(oParenTok, PT_OPAREN))
    return false;

  if (!lex_next_in_range(lexer, ++index))
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
    {
      for (size_t i = 0; i < funcArgs.count; ++i)
      {
        if (ss_compare(&currTok->as.symbol, &funcArgs.items[i]))
        {
          S_ERROR_SS(currTok->cursor, "Function argument '" SS_FMT "' already used!", &currTok->as.symbol);
          // TODO: Maybe provide additional error informations, so the normal parser will not get executed.
          return false;
        }
      }

      node_func_definition_args_append(arena, &funcArgs, currTok->as.symbol);
    }
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

  token_t *assignTok = lex_at(lexer, ++index);

  if (!tok_is_specific_literal(assignTok, CLT_EQUALS))
    return false;

  if (!lex_next_in_range(lexer, index))
  {
    S_ERROR(assignTok->cursor, "After a variable assignment must be an expression!");
    return false;
  }

  ++index;
  node_t *value = parser_execute(varStack, arena, lexer, &index);
  if (!value)
  {
    S_ERROR(assignTok->cursor, "After a variable assignment must be a valid expression");
    return false;
  }

  if (!check_variable_name_errors(varStack, &varNameTok->as.symbol, varNameTok->cursor))
    return false;

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

void node_variable_stack_print(node_variable_stack_t *varStack, bool showValue)
{
  ASSERT_NULL(varStack);

  if (varStack->count <= 0)
  {
    printf("Variable-Stack is empty.\n");
    return;
  }

  printf("Printing variables (%zu):\n", varStack->count);

  for (size_t i = 0; i < varStack->count; ++i)
  {
    node_variable_t *variable = &varStack->items[i];

    switch (variable->type)
    {
    case NV_CONST_VARIABLE_DEFINITION:
    {
      printf(SS_FMT, SS_ARG(&variable->as.varDef));
    }
    break;
    case NV_FUNCTION_DEFINITION:
    {
      printf(SS_FMT "(", SS_ARG(&variable->as.funcDef.name));

      for (size_t i = 0; i < variable->as.funcDef.args.count; ++i)
      {
        printf(SS_FMT, SS_ARG(&variable->as.funcDef.args.items[i]));

        if (i < variable->as.funcDef.args.count - 1)
          printf(", ");
      }

      printf(")");
    }
    break;
    case NV_COUNT:
    default:
      UNREACHABLE("Invalid variable-type!");
      break;
    }

    if (!showValue)
      return;

    printf(" = ");
    print_node(varStack, variable->expression, false);
  }
}

static node_t *try_parse_constant(arena_t *arena, lexer_t *lexer, size_t *index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t *token = lex_at(lexer, *index);

  bool hasPrePlus = false;
  bool hasPreMinus = false;

  if (tok_is_operator(token, OP_ADD))
    hasPrePlus = true;
  else if (tok_is_operator(token, OP_SUB))
    hasPreMinus = true;

  if (hasPrePlus || hasPreMinus)
  {
    ++*index;
    token = lex_at(lexer, *index);
  }

  if (tok_is(token, TT_NUMBER))
  {
    node_const_t constant = node_constant_value_ex(token->as.number, hasPreMinus);
    return node_constant(arena, token->cursor, constant);
  }
  else if (tok_is(token, TT_SYMBOL) && cstr_is_math_constant_ex(token->as.symbol.src, token->as.symbol.len))
  {
    // Checks if the symbol is a valid buildin math constant name.
    e_math_constant_type mc = cstr_to_math_constant_type_ex(token->as.symbol.src, token->as.symbol.len);
    if (mc == MC_INVALID)
      UNREACHABLE("Math-Constant-Type not implemented!");

    node_const_t constant = node_constant_buildin_const_ex(mc, hasPreMinus);
    return node_constant(arena, token->cursor, constant);
  }
  else if (tok_is(token, TT_SYMBOL))
  {
    node_const_t constant = node_constant_variable_ex(token->as.symbol, hasPreMinus);
    return node_constant(arena, token->cursor, constant);
  }

  return NULL;
}

static node_t *try_parse_binop(arena_t *arena, lexer_t *lexer, size_t *index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  node_t *lhs = try_parse_constant(arena, lexer, index);
  if (!lhs)
  {
    token_t *token = lex_at(lexer, *index);
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

  e_node_binop_type type = to_local_binop_type(opTok->as.operator);
  return node_binop(arena, opTok->cursor, type, lhs, rhs);
}

static node_t *try_parse_func(arena_t *arena, lexer_t *lexer, size_t *index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  token_t *token = lex_at(lexer, *index);

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
    return NULL;
  }

  return NULL;
}

// TODO:
// Order checking: Brackets -> Exponents -> Multiplication / Division -> Addition / Substraction
// A stack for checking nested expressions in brackets. Also if the brackets are used correctly.
// Paren checking: 2 Open '(' should have 2 Closing ')' after.
// After a constant / number MUST come an operator, function, or closing paren

// IN: "EN + 5 / (5 * 0)"
// AST: "add(EN, divide(5, paren(mult(5, 0))))"
// TODO: Remove index, use internal lexer cursor
node_t *parser_execute(node_variable_stack_t *varStack, arena_t *arena, lexer_t *lexer, size_t *index)
{
  ASSERT_NULL(varStack);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lexer->isError || lexer->count <= 0)
    return NULL;

  if (*index > lexer->count - 1)
    UNREACHABLE("Lexer out of range!");

  // TODO: Rethink!
  if (tok_is(lex_at(lexer, *index), TT_EOL))
    return NULL;

  if (tok_is(lex_at(lexer, *index), TT_EOF))
    return NULL;

  node_t *nodeBinOp = try_parse_binop(arena, lexer, index);
  if (nodeBinOp)
    return nodeBinOp;

  node_t *nodeConst = try_parse_constant(arena, lexer, index);
  if (nodeConst)
    return nodeConst;

  return NULL;
}

double node_constant_to_value(arena_t *arena, node_variable_stack_t *varStack, node_t *node)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(varStack);
  ASSERT_NULL(node);

  if (node->type != NT_CONSTANT)
    UNREACHABLE("Node not constant!");

  node_const_t *constant = &node->as.constant;

  double result = 0.0;

  switch (constant->type)
  {
  case CN_VALUE:
  {
    result = constant->as.value;
  }
  break;
  case CN_BUILDIN_CONST:
  {
    e_math_constant_type mc = constant->as.buildinConstant;
    if (mc == MC_INVALID)
      UNREACHABLE("Invalid math constant type!");
    result = mathConstantTypeValues[mc];
  }
  break;
  case CN_VARIABLE:
  {
    string_slice_t *varName = &constant->as.variableName;

    node_variable_t *variable = find_node_variable_ex(varStack, varName, FVM_CONST);

    if (!variable)
    {
      // TODO: Rethink error handling!
      E_ERROR_SS(node->cursor, "Variable '" SS_FMT "' not found!", varName);
      return 0;
    }

    ast_eval_res_t res = ast_eval(arena, varStack, variable->expression);

    // TODO: Rethink error handling!
    if (res.isError)
      return 0;

    result = res.value;
  }
  break;
  case CN_COUNT:
  default:
    UNREACHABLE("Invalid constant node type!");
  }

  // Handle pre minus of constant
  if (constant->hasPreMinus)
    result *= -1;

  return result;
}

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
      node_const_t constant = node_constant_value(
          node_constant_to_value(arena, varStack, lhs) + node_constant_to_value(arena, varStack, rhs));
      return node_constant(arena, expr->cursor, constant);
    }
    case NO_SUB:
    {
      node_const_t constant = node_constant_value(
          node_constant_to_value(arena, varStack, lhs) - node_constant_to_value(arena, varStack, rhs));
      return node_constant(arena, expr->cursor, constant);
    }
    case NO_MUL:
    {
      node_const_t constant = node_constant_value(
          node_constant_to_value(arena, varStack, lhs) * node_constant_to_value(arena, varStack, rhs));
      return node_constant(arena, expr->cursor, constant);
    }
    case NO_DIV:
    {
      if (node_constant_to_value(arena, varStack, rhs) == 0)
      {
        E_ERROR(rhs->cursor, "Tried to divide by zero!");
        return NULL;
      }
      node_const_t constant = node_constant_value(
          node_constant_to_value(arena, varStack, lhs) / node_constant_to_value(arena, varStack, rhs));
      return node_constant(arena, expr->cursor, constant);
    }
    case NO_POW:
    {
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

  node_t *evaluatedNode = ast_eval_ex(arena, varStack, expr);
  if (!evaluatedNode)
  {
    G_ERROR("Evaluation failed!");
    return (ast_eval_res_t){
        .isError = true,
    };
  }

  if (evaluatedNode->type != NT_CONSTANT)
    UNREACHABLE("Evaluation failed! Return-Type not of type 'CONSTANT'.");

  if (evaluatedNode->as.constant.type != CN_VALUE)
    UNREACHABLE("Evaluation failed! Return-Type of constant not of type 'VALUE'.");

  return (ast_eval_res_t){
      .value = evaluatedNode->as.constant.as.value,
  };
}

#define _PRINT_DEPTH_SPACES(indented, depth) \
  do                                         \
  {                                          \
    if (indented)                            \
      for (size_t i = 0; i < (depth); ++i)   \
        printf(" ");                         \
  } while (0)

static void print_node_ex(node_variable_stack_t *vs, node_t *node, bool indented, size_t depth)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(node);

  switch (node->type)
  {
  case NT_CONSTANT:
  {
    _PRINT_DEPTH_SPACES(indented, depth);

    if (node->as.constant.hasPreMinus)
      printf("-");

    switch (node->as.constant.type)
    {
    case CN_VALUE:
    {
      printf(DOUBLE_PRINT_FORMAT, node->as.constant.as.value);
    }
    break;
    case CN_BUILDIN_CONST:
    {
      e_math_constant_type mc = node->as.constant.as.buildinConstant;
      if (mc == MC_INVALID)
        UNREACHABLE("Invalid math constant type!");

      // TODO: Rethink!
      if (node->as.constant.hasPreMinus)
        printf("%s (-" DOUBLE_PRINT_FORMAT ", %s)", mathConstantTypeIdentifiers[mc], mathConstantTypeValues[mc], mathConstantTypeNames[mc]);
      else
        printf("%s (" DOUBLE_PRINT_FORMAT ", %s)", mathConstantTypeIdentifiers[mc], mathConstantTypeValues[mc], mathConstantTypeNames[mc]);
    }
    break;
    case CN_VARIABLE:
    {
      // TODO: Rethink!
      bool indentedInternal = indented; // false;
      printf(SS_FMT " =%s", SS_ARG(&node->as.constant.as.variableName), indentedInternal ? "\n" : " ");

      node_variable_t *variable = find_node_variable_ex(vs, &node->as.constant.as.variableName, FVM_CONST);
      if (!variable)
      {
        _PRINT_DEPTH_SPACES(indentedInternal, depth + 1);
        printf("Variable not found!");
        break;
      }

      print_node_ex(vs, variable->expression, indentedInternal, depth + 1);
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
    print_node_ex(vs, node->as.binop.lhs, indented, depth + 1);
    printf(",%s", indented ? "\n" : " ");
    print_node_ex(vs, node->as.binop.rhs, indented, depth + 1);
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

      print_node_ex(vs, args->items[0], indented, !isArgTypeConst ? depth + 1 : 0);

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
        print_node_ex(vs, args->items[i], indented, depth + 1);
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

    print_node_ex(vs, node->as.paren.arg, indented, !isArgTypeConst ? depth + 1 : 0);

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

void print_node(node_variable_stack_t *vs, node_t *node, bool indented)
{
  print_node_ex(vs, node, indented, 0);
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
    print_node(varStack, test->expressionTree, indented);
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
      G_ERROR("Evaluation should have failed but did not!");

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
      if (freeArena)
        arena_free(arena);

      return false;
    }

    if (freeArena)
      arena_free(arena);

    printf("Result      = " DOUBLE_PRINT_FORMAT "\n", evalResult.value);

    if (!compare_doubles(evalResult.value, test->expectedResult, 5))
    {
      G_ERROR("Evaluated result not equal to expected value!");
      return false;
    }
  }

  return true;
}

#endif // _PARSER_H_
