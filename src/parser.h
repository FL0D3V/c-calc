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

#define DEBUG_PRINT_TOK_EX_2(tok, idx, d) printf("DEPTH(%zu), TOKEN(%zu) = %s\n", d, idx, tokenTypeNames[(tok)->type])
#define DEBUG_PRINT_TOK_EX_1(tok, idx) printf("TOKEN(%zu) = %s\n", idx, tokenTypeNames[(tok)->type])
#define DEBUG_PRINT_TOK(tok) printf("TOKEN = %s\n", tokenTypeNames[(tok)->type])

#define _PRINT_DEPTH_SPACES(indented, depth) \
  do                                         \
  {                                          \
    if (indented)                            \
      for (size_t i = 0; i < (depth); ++i)   \
        printf(" ");                         \
  } while (0)

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

// Node definitions
typedef struct node node_t;

typedef struct
{
  e_operator_type type;
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
  e_function_type type;
  node_func_args_t args;
} node_function_t;

typedef struct
{
  node_t *arg;
} node_paren_t;

typedef union
{
  // TODO: Change to big number type!
  double value;
  e_math_constant_type buildinConstant;
  string_slice_t variableName;
} u_const_node_as;

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

typedef struct
{
  e_const_node_type type;
  u_const_node_as as;
  // Indicates that the value should be negated like "5" -> "-5".
  bool hasPreMinus;
} node_const_t;

node_const_t node_constant_value_ex(double val, bool minus)
{
  return (node_const_t){.type = CN_VALUE, .as.value = val, .hasPreMinus = minus};
}
#define node_constant_value(val) node_constant_value_ex(val, false)

node_const_t node_constant_buildin_const_ex(e_math_constant_type val, bool minus)
{
  return (node_const_t){.type = CN_BUILDIN_CONST, .as.buildinConstant = val, .hasPreMinus = minus};
}
#define node_constant_buildin_const(val) node_constant_buildin_const_ex(val, false)

node_const_t node_constant_variable_ex(string_slice_t val, bool minus)
{
  return (node_const_t){.type = CN_VARIABLE, .as.variableName = val, .hasPreMinus = minus};
}
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
  return newNode;
}

node_t *node_constant(arena_t *arena, size_t cursor, node_const_t constant)
{
  node_t *node = base_node(arena, cursor, NT_CONSTANT);
  node->as.constant = constant;
  return node;
}

node_t *node_binop(arena_t *arena, size_t cursor, e_operator_type type, node_t *lhs, node_t *rhs)
{
  assert(type < OP_COUNT);
  node_t *node = base_node(arena, cursor, NT_BINOP);
  node->as.binop.type = type;
  node->as.binop.lhs = lhs;
  node->as.binop.rhs = rhs;
  return node;
}

node_t *node_func(arena_t *arena, size_t cursor, e_function_type type, node_func_args_t args)
{
  assert(type < FT_COUNT);
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

typedef struct
{
  // Each indivually defined function argument name like "x,y,z", or "a1, a2, a3", ...
  // These are locally available in the given expression as variable symbols.
  // They will be replaced with the corresponding values when the function is called.
  string_slice_t *items;
  size_t capacity;
  size_t count;
} node_func_definition_args_t;

#define node_func_args_at(args, idx) (&(args)->items[(idx)])

#define node_func_definition_args_append(a, funcArgs, arg) \
  arena_da_append(a, funcArgs, arg)

typedef struct
{
  // The name of the function. With this name the function can be called in an expression.
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

  // Count of variable names.
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

#define node_variable_constant(varName, value) \
  ((node_variable_t){.type = NV_CONST_VARIABLE_DEFINITION, .as.varDef = (varName), .expression = (value)})
#define node_variable_function(varName, funcArgs, value) \
  ((node_variable_t){.type = NV_FUNCTION_DEFINITION, .as.funcDef.name = (varName), .as.funcDef.args = (funcArgs), .expression = (value)})

typedef struct
{
  node_variable_t *items;
  size_t capacity;
  size_t count;
} node_variable_stack_t;

#define vs_variable_append(a, vs, var) \
  arena_da_append(a, vs, var)

#define vs_at(vs, idx) (&(vs)->items[idx])

typedef enum
{
  FVM_ALL,
  FVM_CONST,
  FVM_FUNC,

  FVM_COUNT
} e_find_var_mode;

static_assert(FVM_COUNT == 3, "Amount of find node var modes have changed!");

node_variable_t *vs_find_variable_ex(node_variable_stack_t *vs, string_slice_t *name, e_find_var_mode mode)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(name);

  for (size_t i = 0; i < vs->count; ++i)
  {
    node_variable_t *curr = vs_at(vs, i);
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
        UNREACHABLE("Invalid node-variable-type!");
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
      UNREACHABLE("Invalid find-var-mode!");
    }

    if (!currName)
      continue;

    // Check if the given name is in the var stack.
    if (ss_compare(name, currName))
      return curr;
  }

  return NULL;
}

#define vs_find_variable(vs, name) \
  vs_find_variable_ex((vs), (name), FVM_ALL)

bool vs_variable_name_exists_ex(node_variable_stack_t *vs, string_slice_t *name, e_find_var_mode mode)
{
  node_variable_t *v = vs_find_variable_ex(vs, name, mode);
  return v != NULL;
}

#define vs_variable_name_exists(vs, name) \
  vs_variable_name_exists_ex((vs), (name), FVM_ALL)

static bool vs_check_variable_name_errors(node_variable_stack_t *vs, string_slice_t *name, size_t cursor)
{
  if (vs_variable_name_exists(vs, name))
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

static bool try_parse_const_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer);
static bool try_parse_func_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer);
bool parse_node_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer);
void print_variable_stack(node_variable_stack_t *vs, bool showValue, bool indented);

static node_t *try_parse_constant(arena_t *arena, lexer_t *lexer, size_t *index);
static node_t *try_parse_binop(arena_t *arena, lexer_t *lexer, size_t *index, size_t *depth);
static node_t *try_parse_paren(arena_t *arena, lexer_t *lexer, size_t *index, size_t *depth);
static node_t *try_parse_func(arena_t *arena, lexer_t *lexer, size_t *index, size_t *depth);

node_t *parser_execute_ex(arena_t *arena, lexer_t *lexer, size_t *index);
node_t *parser_execute(arena_t *arena, lexer_t *lexer);
static void print_node_ex(node_variable_stack_t *vs, node_t *node, bool indented, size_t depth);
void print_node(node_variable_stack_t *vs, node_t *node, bool indented);
void print_node_simple(node_t *node, bool indented);

node_t *ast_eval_ex(arena_t *arena, node_variable_stack_t *vs, node_t *expr);

typedef struct
{
  // TODO: Change to big number!
  double value;
  bool isError;
} ast_eval_res_t;

#define ast_eval_res_success(val) \
  (ast_eval_res_t) { .value = (val) }
#define ast_eval_res_error() \
  (ast_eval_res_t) { .isError = true }

ast_eval_res_t ast_eval(arena_t *arena, node_variable_stack_t *vs, node_t *expr);

static bool try_parse_const_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (!lex_valid(lexer))
    return true;

  size_t index = 0;

  token_t *varNameTok = lex_at(lexer, index);

  if (!tok_is(varNameTok, TT_SYMBOL))
    return false;

  if (!lex_next_in_range(lexer, index))
    return false;

  ++index;

  token_t *assignTok = lex_at(lexer, index);

  if (tok_is_end(assignTok))
    return true;

  if (!tok_is_specific_literal(assignTok, CLT_EQUALS))
    return false;

  if (!lex_next_in_range(lexer, index))
  {
    S_ERROR(assignTok->cursor, "After a variable assignment must be an expression!");
    return false;
  }

  ++index;

  node_t *value = parser_execute_ex(arena, lexer, &index);
  if (!value)
  {
    S_ERROR(assignTok->cursor, "After a variable assignment must be a valid expression!");
    return false;
  }

  // Checks if the variable name is already used
  if (!vs_check_variable_name_errors(vs, &varNameTok->as.symbol, varNameTok->cursor))
    return false;

  // Create and add a new variable.
  node_variable_t constantVar = node_variable_constant(varNameTok->as.symbol, value);
  vs_variable_append(arena, vs, constantVar);
  return true;
}

static bool try_parse_func_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (!lex_valid(lexer))
    return true;

  size_t index = 0;

  token_t *varNameTok = lex_at(lexer, index);

  if (!tok_is(varNameTok, TT_SYMBOL))
    return false;

  if (!lex_next_in_range(lexer, index))
    return false;

  ++index;

  token_t *oParenTok = lex_at(lexer, index);

  if (tok_is_end(oParenTok))
    return false;

  if (!tok_is_paren(oParenTok, PT_OPAREN))
    return false;

  if (!lex_next_in_range(lexer, index))
  {
    S_ERROR(oParenTok->cursor, "Expected at least one function argument name and then a closing paren and an assigned expression!");
    return false;
  }

  node_func_definition_args_t funcArgs = {0};
  token_t *currTok = NULL;

  do
  {
    ++index;

    currTok = lex_at(lexer, index);

    if (tok_is_end(currTok))
      return false;

    if (tok_is(currTok, TT_SYMBOL))
    {
      // Check if function argument is already used in definition.
      for (size_t i = 0; i < funcArgs.count; ++i)
      {
        if (ss_compare(&currTok->as.symbol, &funcArgs.items[i]))
        {
          S_ERROR_SS(currTok->cursor, "Function argument '" SS_FMT "' already used!", &currTok->as.symbol);
          return false;
        }
      }

      // Add the argument.
      node_func_definition_args_append(arena, &funcArgs, currTok->as.symbol);
    }
    else if (tok_is_specific_literal(currTok, CLT_COMMA))
      continue;
    else
      break;
  } while (!tok_is_end(currTok) || lex_next_in_range(lexer, index));

  if (tok_is_end(currTok) || !lex_next_in_range(lexer, index))
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

  ++index;

  token_t *assignTok = lex_at(lexer, index);

  if (tok_is_end(assignTok))
    return false;

  if (!tok_is_specific_literal(assignTok, CLT_EQUALS))
    return false;

  if (!lex_next_in_range(lexer, index))
  {
    S_ERROR(assignTok->cursor, "After a variable assignment must be an expression!");
    return false;
  }

  ++index;

  node_t *value = parser_execute_ex(arena, lexer, &index);
  if (!value)
  {
    S_ERROR(assignTok->cursor, "After a variable assignment must be a valid expression");
    return false;
  }

  // Checks if the variable name is already used
  if (!vs_check_variable_name_errors(vs, &varNameTok->as.symbol, varNameTok->cursor))
    return false;

  // Create and add a new variable.
  node_variable_t funcVar = node_variable_function(varNameTok->as.symbol, funcArgs, value);
  vs_variable_append(arena, vs, funcVar);
  return true;
}

bool parse_node_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (!lex_valid(lexer))
    return true;

  bool shouldExecuteVarParsing = false;

  // TODO: Handle multiline handling
  for (size_t i = 0; i < lexer->count; ++i)
  {
    token_t *token = lex_at(lexer, i);
    if (tok_is_end(token))
      break;
    if (tok_is_specific_literal(token, CLT_EQUALS))
      shouldExecuteVarParsing = true;
  }

  if (!shouldExecuteVarParsing)
    return true;

  return try_parse_const_variable(vs, arena, lexer) ||
         try_parse_func_variable(vs, arena, lexer);
}

void print_variable_stack(node_variable_stack_t *vs, bool showValue, bool indented)
{
  ASSERT_NULL(vs);

  if (vs->count <= 0)
  {
    printf("Variable-Stack is empty.\n");
    return;
  }

  printf("Printing variables (%zu):\n", vs->count);

  for (size_t i = 0; i < vs->count; ++i)
  {
    node_variable_t *variable = vs_at(vs, i);

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

    printf(" =%s", indented ? "\n" : " ");
    print_node_ex(vs, variable->expression, indented, 2);
    printf("\n");
    if (indented && i < vs->count - 1)
      printf("\n");
  }
}

static node_t *try_parse_constant(arena_t *arena, lexer_t *lexer, size_t *index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(index);

  if (!lex_valid(lexer))
  {
    fprintf(stderr, "ERROR: Lexer invalid! (index=%zu, count=%zu)\n", *index, lexer->count);
    return NULL;
  }

  if (!lex_in_range(lexer, *index))
    return NULL;

  token_t *token = lex_at(lexer, *index);

  bool hasPrePlus = false;
  bool hasPreMinus = false;

  if (tok_is_operator(token, OP_SUB))
    hasPreMinus = true;
  else if (tok_is_operator(token, OP_ADD))
    hasPrePlus = true;

  if (hasPreMinus || hasPrePlus)
  {
    if (!lex_next_in_range(lexer, *index))
    {
      S_ERROR(token->cursor, "Expected a value after!");
      return NULL;
    }

    ++(*index);

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
    // TODO: Maybe link variable directly! This means that all variables must be parsed first!
    node_const_t constant = node_constant_variable_ex(token->as.symbol, hasPreMinus);
    return node_constant(arena, token->cursor, constant);
  }

  return NULL;
}

// TODO: Implement proper precedence handling!
static node_t *try_parse_binop(arena_t *arena, lexer_t *lexer, size_t *index, size_t *depth)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(index);
  ASSERT_NULL(depth);

  if (!lex_valid(lexer))
  {
    fprintf(stderr, "ERROR: Lexer invalid! (index=%zu, count=%zu)\n", *index, lexer->count);
    return NULL;
  }

  if (!lex_in_range(lexer, *index))
    return NULL;

  node_t *lhs = NULL;

  lhs = try_parse_func(arena, lexer, index, depth);

  if (!lhs)
    lhs = try_parse_paren(arena, lexer, index, depth);

  if (!lhs)
  {
    lhs = try_parse_constant(arena, lexer, index);

    if (!lex_next_in_range(lexer, *index))
      return lhs;

    ++(*index);
  }

  if (!lhs)
  {
    S_ERROR(lex_at(lexer, *index)->cursor, "Expected a constant value!");
    return NULL;
  }

  token_t *opToken = lex_at(lexer, *index);

  if (tok_is_end(opToken))
    return lhs;

  // Used for ending the paren or function parsing.
  if (depth != 0 && (tok_is_paren(opToken, PT_CPAREN) || tok_is_specific_literal(opToken, CLT_COMMA)))
    return lhs;

  if (!tok_is(opToken, TT_OPERATOR))
  {
    S_ERROR(opToken->cursor, "Expected an operator!");
    return NULL;
  }

  e_operator_type op = opToken->as.operator;

  ++(*index);

  // Try to parse the next operator.
  node_t *rhs = try_parse_binop(arena, lexer, index, depth);
  if (!rhs)
  {
    S_ERROR(opToken->cursor, "Invalid symbol found after operator!");
    return NULL;
  }

  return node_binop(arena, opToken->cursor, op, lhs, rhs);
}

static node_t *try_parse_paren(arena_t *arena, lexer_t *lexer, size_t *index, size_t *depth)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(index);
  ASSERT_NULL(depth);

  if (!lex_valid(lexer) || !lex_in_range(lexer, *index))
  {
    fprintf(stderr, "ERROR: Lexer invalid! (index=%zu, count=%zu)\n", *index, lexer->count);
    return NULL;
  }

  token_t *tokOParen = lex_at(lexer, *index);

  if (!tok_is_paren(tokOParen, PT_OPAREN))
    return NULL;

  if (!lex_next_in_range(lexer, *index))
  {
    S_ERROR(tokOParen->cursor, "Expected an expression after an open paren!");
    return NULL;
  }

  ++(*index);

  token_t *token = lex_at(lexer, *index);

  if (tok_is_end(token))
  {
    S_ERROR(tokOParen->cursor, "Expected an expression after an open paren!");
    return NULL;
  }

  ++(*depth);

  node_t *arg = try_parse_binop(arena, lexer, index, depth);
  if (!arg)
    return NULL;

  --(*depth);

  token = lex_at(lexer, *index);

  if (tok_is_end(token) || tok_not_specific_paren(token, PT_CPAREN))
  {
    S_ERROR(tokOParen->cursor, "Expected a closing paren!");
    return NULL;
  }

  ++(*index);

  return node_paren(arena, tokOParen->cursor, arg);
}

// TODO: Handle user defined functions!
static node_t *try_parse_func(arena_t *arena, lexer_t *lexer, size_t *index, size_t *depth)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(index);
  ASSERT_NULL(depth);

  if (!lex_valid(lexer) || !lex_in_range(lexer, *index))
  {
    fprintf(stderr, "ERROR: Lexer invalid! (index=%zu, count=%zu)\n", *index, lexer->count);
    return NULL;
  }

  token_t *funcNameTok = lex_at(lexer, *index);

  if (!tok_is(funcNameTok, TT_SYMBOL))
    return NULL;

  // Checks if the symbol is a valid function name.
  e_function_type ft = ss_to_function_type(&funcNameTok->as.symbol);
  if (ft == FT_INVALID)
    return NULL;

  // Next token must be an open paren.
  if (!lex_next_in_range(lexer, *index))
    return NULL;

  ++(*index);

  token_t *nextTok = lex_at(lexer, *index);

  if (!tok_is_paren(nextTok, PT_OPAREN))
    return NULL;

  node_func_args_t argsList = {0};

  do
  {
    if (!lex_next_in_range(lexer, *index))
      return NULL;

    token_t *lastTok = nextTok;

    ++(*index);

    nextTok = lex_at(lexer, *index);

    if (tok_is_end(nextTok))
    {
      S_ERROR(lastTok->cursor, "Expected a valid function!");
      return NULL;
    }

    ++(*depth);

    node_t *arg = try_parse_binop(arena, lexer, index, depth);
    if (!arg)
    {
      S_ERROR(nextTok->cursor, "Expected at least one function argument!");
      return NULL;
    }

    --(*depth);

    nextTok = lex_at(lexer, *index);

    // Add argument to list.
    node_func_args_append(arena, &argsList, arg);
  } while (!tok_is_end(nextTok) && !tok_is_paren(nextTok, PT_CPAREN) && tok_is_specific_literal(nextTok, CLT_COMMA));

  if (tok_is_end(nextTok) || tok_not_specific_paren(nextTok, PT_CPAREN))
  {
    S_ERROR(nextTok->cursor, "Expected a closing paren!");
    return NULL;
  }

  ++(*index);

  return node_func(arena, funcNameTok->cursor, ft, argsList);
}

// TODO: Order checking. Brackets -> Exponents -> Multiplication / Division -> Addition / Substraction
// TODO: Remove index, use internal lexer cursor
// IN: "EN + 5 / (5 * 0)"
// AST: "add(EN, divide(5, paren(mult(5, 0))))"
node_t *parser_execute_ex(arena_t *arena, lexer_t *lexer, size_t *index)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (!lex_valid(lexer) || !lex_in_range(lexer, *index))
  {
    fprintf(stderr, "ERROR: Lexer invalid! (index=%zu, count=%zu)\n", *index, lexer->count);
    return NULL;
  }

  token_t *currTok = lex_at(lexer, *index);

  // These are end tokens signaling that a specific end is reached at the current index.
  if (tok_is_end(currTok))
    return NULL;

  size_t depth = 0;

  node_t *root = try_parse_binop(arena, lexer, index, &depth);

  if (depth != 0)
    UNREACHABLE("Paren parsing failed!");

  return root;
}

node_t *parser_execute(arena_t *arena, lexer_t *lexer)
{
  size_t index = 0;
  return parser_execute_ex(arena, lexer, &index);
}

double node_constant_to_value(arena_t *arena, node_variable_stack_t *vs, node_t *node, bool *isError)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(vs);
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
    node_variable_t *variable = vs_find_variable_ex(vs, varName, FVM_CONST);
    if (!variable)
    {
      *isError = true;
      E_ERROR_SS(node->cursor, "Variable '" SS_FMT "' not found!", varName);
      return 0;
    }

    ast_eval_res_t res = ast_eval(arena, vs, variable->expression);
    if (res.isError)
    {
      *isError = true;
      return 0;
    }

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

node_t *ast_eval_binop(arena_t *arena, node_variable_stack_t *vs, size_t cursor, node_t *lhs, node_t *rhs, e_operator_type op)
{
  bool isError = false;

  double left = node_constant_to_value(arena, vs, lhs, &isError);
  if (isError)
    return NULL;

  double right = node_constant_to_value(arena, vs, rhs, &isError);
  if (isError)
    return NULL;

  switch (op)
  {
  case OP_ADD:
  {
    return node_constant(arena, cursor, node_constant_value(left + right));
  }
  case OP_SUB:
  {
    return node_constant(arena, cursor, node_constant_value(left - right));
  }
  case OP_MUL:
  {
    return node_constant(arena, cursor, node_constant_value(left * right));
  }
  case OP_DIV:
  {
    if (right == 0)
    {
      E_ERROR(rhs->cursor, "Tried to divide by zero!");
      return NULL;
    }
    return node_constant(arena, cursor, node_constant_value(left / right));
  }
  case OP_POW:
  {
    return node_constant(arena, cursor, node_constant_value(pow(left, right)));
  }
  case OP_COUNT:
  case OP_INVALID:
  default:
    UNREACHABLE("Invalid operator-type!");
  }
}

node_t *ast_eval_ex(arena_t *arena, node_variable_stack_t *vs, node_t *expr)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(vs);
  ASSERT_NULL(expr);

  bool isError = false;

  switch (expr->type)
  {
  case NT_CONSTANT:
  {
    // Will evaluate down to a constant value type.
    double value = node_constant_to_value(arena, vs, expr, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, expr->cursor, node_constant_value(value));
  }
  case NT_BINOP:
  {
    node_t *lhs = ast_eval_ex(arena, vs, expr->as.binop.lhs);
    if (!lhs)
      return NULL;

    node_t *rhs = ast_eval_ex(arena, vs, expr->as.binop.rhs);
    if (!rhs)
      return NULL;

    return ast_eval_binop(arena, vs, expr->cursor, lhs, rhs, expr->as.binop.type);
  }
  case NT_FUNCTION:
  {
    switch (expr->as.func.type)
    {
    case FT_SQRT:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "sqrt() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(sqrt(value)));
    }
    case FT_EXP:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "exp() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(exp(value)));
    }
    case FT_SIN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "sin() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(sin(value)));
    }
    case FT_ASIN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "asin() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(asin(value)));
    }
    case FT_SINH:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "sinh() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(sinh(value)));
    }
    case FT_COS:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "cos() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(cos(value)));
    }
    case FT_ACOS:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "acos() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(acos(value)));
    }
    case FT_COSH:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "cosh() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(cosh(value)));
    }
    case FT_TAN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "tanh() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(tan(value)));
    }
    case FT_ATAN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "atan() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(atan(value)));
    }
    case FT_TANH:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "tanh() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(tanh(value)));
    }
    case FT_ATAN2:
    {
      if (expr->as.func.args.count != 2)
      {
        E_ERROR(expr->cursor, "atan2() takes two arguments!");
        return NULL;
      }
      node_t *arg1 = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg1)
        return NULL;
      node_t *arg2 = ast_eval_ex(arena, vs, expr->as.func.args.items[1]);
      if (!arg2)
        return NULL;
      double value1 = node_constant_to_value(arena, vs, arg1, &isError);
      if (isError)
        return NULL;
      double value2 = node_constant_to_value(arena, vs, arg2, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(atan2(value1, value2)));
    }
    case FT_LN:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "ln() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(log(value)));
    }
    case FT_LOG10:
    {
      if (expr->as.func.args.count != 1)
      {
        E_ERROR(expr->cursor, "log10() takes one argument!");
        return NULL;
      }
      node_t *arg = ast_eval_ex(arena, vs, expr->as.func.args.items[0]);
      if (!arg)
        return NULL;
      double value = node_constant_to_value(arena, vs, arg, &isError);
      if (isError)
        return NULL;
      return node_constant(arena, expr->cursor, node_constant_value(log10(value)));
    }
    case FT_COUNT:
    case FT_INVALID:
    default:
      UNREACHABLE("Invalid function-type!");
    }
    break;
  }
  case NT_PAREN:
  {
    node_t *arg = ast_eval_ex(arena, vs, expr->as.paren.arg);
    if (!arg)
      return NULL;
    return node_constant(arena, expr->cursor, arg->as.constant);
  }
  case NT_COUNT:
  default:
    UNREACHABLE("Invalid node-type!");
  }
}

ast_eval_res_t ast_eval(arena_t *arena, node_variable_stack_t *vs, node_t *expr)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(vs);
  ASSERT_NULL(expr);

  node_t *evaluatedNode = ast_eval_ex(arena, vs, expr);
  if (!evaluatedNode)
    return ast_eval_res_error();

  if (evaluatedNode->type != NT_CONSTANT)
    UNREACHABLE("Evaluation failed! Return-Type not of type 'CONSTANT'.");

  if (evaluatedNode->as.constant.type != CN_VALUE)
    UNREACHABLE("Evaluation failed! Return-Type of constant not of type 'VALUE'.");

  return ast_eval_res_success(evaluatedNode->as.constant.as.value);
}

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

      if (node->as.constant.hasPreMinus)
        printf("%s (-" DOUBLE_PRINT_FORMAT ", %s)", mathConstantTypeIdentifiers[mc], mathConstantTypeValues[mc], mathConstantTypeNames[mc]);
      else
        printf("%s (" DOUBLE_PRINT_FORMAT ", %s)", mathConstantTypeIdentifiers[mc], mathConstantTypeValues[mc], mathConstantTypeNames[mc]);
    }
    break;
    case CN_VARIABLE:
    {
      bool indentedInternal = indented;
      printf("'" SS_FMT "' =%s", SS_ARG(&node->as.constant.as.variableName), indentedInternal ? "\n" : " ");

      node_variable_t *variable = vs_find_variable_ex(vs, &node->as.constant.as.variableName, FVM_CONST);
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
    if (node->as.binop.type >= OP_COUNT)
      UNREACHABLE("Invalid operator-type!");

    _PRINT_DEPTH_SPACES(indented, depth);
    printf("%s(", operatorTypeNames[node->as.binop.type]);
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
    if (funcNode->type >= FT_COUNT)
      UNREACHABLE("Invalid function-type!");

    node_func_args_t *args = &funcNode->args;

    if (args->count <= 0)
      UNREACHABLE("No arguments found in function node!");

    _PRINT_DEPTH_SPACES(indented, depth);

    printf("%s(", functionTypeIdentifiers[funcNode->type]);

    if (args->count == 1)
    {
      node_t *arg = args->items[0];

      bool isArgTypeConst = arg->type == NT_CONSTANT && arg->as.constant.type != CN_VARIABLE;

      if (indented && !isArgTypeConst)
        printf("\n");

      print_node_ex(vs, arg, indented, !isArgTypeConst ? depth + 1 : 0);

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
    node_paren_t *parenNode = &node->as.paren;
    node_t *arg = parenNode->arg;

    _PRINT_DEPTH_SPACES(indented, depth);

    printf("paren(");

    bool isArgTypeConst = arg->type == NT_CONSTANT && arg->as.constant.type != CN_VARIABLE;

    if (indented && !isArgTypeConst)
      printf("\n");

    print_node_ex(vs, arg, indented, !isArgTypeConst ? depth + 1 : 0);

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
  printf("Printing node tree:\n");
  print_node_ex(vs, node, indented, 0);
  printf("\n");
}

void print_node_simple(node_t *node, bool indented)
{
  node_variable_stack_t vs = {0};
  print_node(&vs, node, indented);
}

typedef struct
{
  const char *input;
  double expectedResult;
  bool shouldFail;
  node_t *expressionTree;
} node_test_t;

static bool handle_test_result(arena_t *arena, node_test_t *test, const char *name, ast_eval_res_t *evalResult, bool freeArena)
{
  if (test->shouldFail)
  {
    if (!evalResult->isError)
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
    if (freeArena)
      arena_free(arena);

    if (evalResult->isError)
      return false;

    printf("%s = " DOUBLE_PRINT_FORMAT "\n", name, evalResult->value);

    if (!compare_doubles(evalResult->value, test->expectedResult, 5))
    {
      G_ERROR("Evaluated result not equal to expected value!");
      return false;
    }
  }

  return true;
}

bool execute_test(arena_t *arena, node_variable_stack_t *vs, node_test_t *test, bool freeArena, bool verbose, bool indented)
{
  tokenizer_t tokenizer = tokenizer_execute(arena, test->input);
  if (tokenizer.isError)
  {
    if (freeArena)
      arena_free(arena);

    return false;
  }

  lexer_t lexer = lexer_execute(arena, &tokenizer);
  if (lexer.isError)
  {
    if (freeArena)
      arena_free(arena);

    return false;
  }

  node_t *generatedTree = parser_execute(arena, &lexer);
  if (!generatedTree)
  {
    if (freeArena)
      arena_free(arena);

    return false;
  }

  if (verbose)
  {
    printf("\n");
    printf("Internal tree:");
    printf("\n");
    print_node(vs, test->expressionTree, indented);
    printf("\n");

    printf("Generated tree:");
    printf("\n");
    print_node(vs, generatedTree, indented);
    printf("\n");
  }

  printf("Input           = %s\n", test->input);

  if (!test->shouldFail)
    printf("Expected        = " DOUBLE_PRINT_FORMAT "\n", test->expectedResult);

  ast_eval_res_t evalResultGivenTree = ast_eval(arena, vs, test->expressionTree);
  ast_eval_res_t evalResultGeneratedTree = ast_eval(arena, vs, generatedTree);

  bool res1 = handle_test_result(arena, test, "Given-Tree      ", &evalResultGivenTree, freeArena);
  bool res2 = handle_test_result(arena, test, "Generated-Tree  ", &evalResultGeneratedTree, freeArena);

  return res1 && res2;
}

#endif // _PARSER_H_
