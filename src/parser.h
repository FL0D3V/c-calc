#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"

#define _G_ERROR_NAME "ERROR"
#define _S_ERROR_NAME "SEMANTIC-ERROR"
#define _E_ERROR_NAME "EVALUATION-ERROR"
#define _PARSER_ERROR_EX_2(type, cursor, message, arg1, arg2) \
  fprintf(stderr, type ":%zu: " message "\n", cursor, arg1, arg2)
#define _PARSER_ERROR_EX(type, cursor, message, arg) \
  fprintf(stderr, type ":%zu: " message "\n", cursor, arg)
#define _PARSER_ERROR(type, cursor, message) \
  fprintf(stderr, type ":%zu: %s\n", cursor, message)
#define _PARSER_ERROR_SS_2(type, cursor, message, ss, arg) \
  fprintf(stderr, type ":%zu: " message "\n", cursor, SS_ARG(ss), arg)
#define _PARSER_ERROR_SS(type, cursor, message, ss) \
  fprintf(stderr, type ":%zu: " message "\n", cursor, SS_ARG(ss))
#define G_ERROR(message) \
  fprintf(stderr, _G_ERROR_NAME ": %s\n", message)
#define S_ERROR_EX(cursor, message, arg) _PARSER_ERROR_EX(_S_ERROR_NAME, cursor, message, arg)
#define S_ERROR(cursor, message) _PARSER_ERROR(_S_ERROR_NAME, cursor, message)
#define S_ERROR_SS(cursor, message, ss) _PARSER_ERROR_SS(_S_ERROR_NAME, cursor, message, ss)
#define E_ERROR_EX_2(cursor, message, arg1, arg2) _PARSER_ERROR_EX_2(_E_ERROR_NAME, cursor, message, arg1, arg2)
#define E_ERROR_EX(cursor, message, arg) _PARSER_ERROR_EX(_E_ERROR_NAME, cursor, message, arg)
#define E_ERROR(cursor, message) _PARSER_ERROR(_E_ERROR_NAME, cursor, message)
#define E_ERROR_SS_2(cursor, message, ss, arg) _PARSER_ERROR_SS_2(_E_ERROR_NAME, cursor, message, ss, arg)
#define E_ERROR_SS(cursor, message, ss) _PARSER_ERROR_SS(_E_ERROR_NAME, cursor, message, ss)

// TODO: Maybe remove!
#define DEBUG_PRINT_TOK_EX_2(tok, idx, d) printf("DEPTH(%zu), TOKEN(%zu) = %s\n", d, idx, tokenTypeNames[(tok)->type])
#define DEBUG_PRINT_TOK_EX_1(tok, idx) printf("TOKEN(%zu) = %s\n", idx, tokenTypeNames[(tok)->type])
#define DEBUG_PRINT_TOK(tok) printf("TOKEN = %s\n", tokenTypeNames[(tok)->type])

#define _PRINT_DEPTH_SPACES(indented, depth) \
  do                                         \
  {                                          \
    if (indented)                            \
      for (size_t i = 0; i < (depth); ++i)   \
        printf(" - ");                       \
  } while (0)

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

typedef union
{
  e_function_type buildinFunc;
  string_slice_t variableName;
} u_func_node_as;

typedef enum
{
  FN_BUILDIN_FUNC,
  FN_VARIABLE,

  FN_COUNT
} e_func_node_type;

static_assert(FN_COUNT == 2, "Amount of value node types have changed!");

const char *funcNodeTypeNames[FN_COUNT] = {
    [FN_BUILDIN_FUNC] = "Buildin-Function",
    [FN_VARIABLE] = "Variable",
};

typedef struct
{
  e_func_node_type type;
  u_func_node_as as;
  node_func_args_t args;
} node_func_t;

static inline node_func_t node_func_buildin(e_function_type type, node_func_args_t args)
{
  return (node_func_t){
      .type = FN_BUILDIN_FUNC,
      .as.buildinFunc = type,
      .args = args,
  };
}

static inline node_func_t node_func_variable(string_slice_t val, node_func_args_t args)
{
  return (node_func_t){
      .type = FN_VARIABLE,
      .as.variableName = val,
      .args = args,
  };
}

typedef struct
{
  node_t *child;
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

static_assert(CN_COUNT == 3, "Amount of const-node-types have changed!");

const char *constNodeTypeNames[CN_COUNT] = {
    [CN_VALUE] = "Value",
    [CN_BUILDIN_CONST] = "Math-Constant",
    [CN_VARIABLE] = "Variable",
};

typedef struct
{
  e_const_node_type type;
  u_const_node_as as;
} node_const_t;

static inline node_const_t node_constant_value(double val)
{
  return (node_const_t){
      .type = CN_VALUE,
      .as.value = val,
  };
}

static inline node_const_t node_constant_buildin_const(e_math_constant_type val)
{
  return (node_const_t){
      .type = CN_BUILDIN_CONST,
      .as.buildinConstant = val,
  };
}

static inline node_const_t node_constant_variable(string_slice_t val)
{
  return (node_const_t){
      .type = CN_VARIABLE,
      .as.variableName = val,
  };
}

typedef enum
{
  NPT_PLUS,
  NPT_MINUS,

  NPT_COUNT,
} e_node_prefix_type;

static_assert(NPT_COUNT == 2, "Amount of node-prefix-types have changed!");

const char *nodePrefixTypeNames[NPT_COUNT] = {
    [NPT_PLUS] = "Plus",
    [NPT_MINUS] = "Minus",
};

typedef struct
{
  e_node_prefix_type type;
  node_t *child;
} node_prefix_t;

typedef union
{
  node_prefix_t prefix;
  node_const_t constant;
  node_binop_t binop;
  node_func_t func;
  node_paren_t paren;
} u_node_as;

typedef enum
{
  NT_PREFIX_EXPRESSION,
  NT_CONSTANT,
  NT_BINOP,
  NT_FUNCTION,
  NT_PAREN,

  NT_COUNT
} e_node_type;

static_assert(NT_COUNT == 5, "Amount of node-types have changed");

const char *nodeTypeNames[NT_COUNT] = {
    [NT_PREFIX_EXPRESSION] = "prefix",
    [NT_CONSTANT] = "constant",
    [NT_BINOP] = "operator",
    [NT_FUNCTION] = "function",
    [NT_PAREN] = "parenthesis",
};

struct node
{
  e_node_type type;
  size_t cursor;
  u_node_as as;
};

static node_t *base_node(arena_t *arena, size_t cursor, e_node_type type)
{
  ASSERT_NULL(arena);
  assert(type < NT_COUNT);
  node_t *newNode = (node_t *)arena_alloc(arena, sizeof(node_t));
  *newNode = (node_t){
      .type = type,
      .cursor = cursor,
  };
  return newNode;
}

node_t *node_prefix_expression(arena_t *arena, size_t cursor, e_node_prefix_type type, node_t *child)
{
  ASSERT_NULL(arena);
  node_t *node = base_node(arena, cursor, NT_PREFIX_EXPRESSION);
  node->as.prefix = (node_prefix_t){
      .type = type,
      .child = child,
  };
  return node;
}

node_t *node_constant(arena_t *arena, size_t cursor, node_const_t constant)
{
  ASSERT_NULL(arena);
  node_t *node = base_node(arena, cursor, NT_CONSTANT);
  node->as.constant = constant;
  return node;
}

node_t *node_binop(arena_t *arena, size_t cursor, e_operator_type type, node_t *lhs, node_t *rhs)
{
  ASSERT_NULL(arena);
  assert(type < OP_COUNT);
  ASSERT_NULL(lhs);
  ASSERT_NULL(rhs);
  node_t *node = base_node(arena, cursor, NT_BINOP);
  node->as.binop = (node_binop_t){
      .type = type,
      .lhs = lhs,
      .rhs = rhs,
  };
  return node;
}

node_t *node_func(arena_t *arena, size_t cursor, node_func_t function)
{
  ASSERT_NULL(arena);
  node_t *node = base_node(arena, cursor, NT_FUNCTION);
  node->as.func = function;
  return node;
}

node_t *node_paren(arena_t *arena, size_t cursor, node_t *child)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(child);
  node_t *node = base_node(arena, cursor, NT_PAREN);
  node->as.paren = (node_paren_t){
      .child = child,
  };
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

#define node_func_args_empty(args) ((args)->count == 0)

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
  assert(mode < FVM_COUNT);

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
  if (ss_is_math_constant(name))
  {
    S_ERROR_SS(cursor, "Variable name '" SS_FMT "' overlaps with a buildin math-constant!", name);
    return false;
  }
  if (ss_is_function_type(name))
  {
    S_ERROR_SS(cursor, "Variable name '" SS_FMT "' overlaps with a buildin function!", name);
    return false;
  }
  return true;
}

static bool try_parse_const_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer);
static bool try_parse_func_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer);
bool parse_node_variables(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer);
void print_variable_stack(node_variable_stack_t *vs, bool showValue, bool indented);

static node_t *try_parse_prefix_expression(arena_t *arena, lexer_t *lexer, size_t *depth);
static node_t *try_parse_constant(arena_t *arena, lexer_t *lexer);
static node_t *try_parse_binop(arena_t *arena, lexer_t *lexer, size_t *depth, e_precedence precedence);
static node_t *try_parse_paren(arena_t *arena, lexer_t *lexer, size_t *depth);
static node_t *try_parse_func(arena_t *arena, lexer_t *lexer, size_t *depth);
static node_t *try_parse_primary(arena_t *arena, lexer_t *lexer, size_t *depth);
// static node_t *try_parse_pow_op(arena_t *arena, lexer_t *lexer, size_t *depth);
// static node_t *try_parse_point_op(arena_t *arena, lexer_t *lexer, size_t *depth);
// static node_t *try_parse_line_op(arena_t *arena, lexer_t *lexer, size_t *depth);

node_t *parser_execute(arena_t *arena, lexer_t *lexer);
static void print_node_ex(node_variable_stack_t *vs, node_t *node, bool indented, size_t depth);
void print_node(node_variable_stack_t *vs, node_t *node, bool indented);
void print_node_simple(node_t *node, bool indented);

node_t *ast_eval_ex(arena_t *arena, node_variable_stack_t *vs, node_t *expr);

typedef struct
{
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

  if (!lex_is_valid(lexer) || !lex_at_start_of_line(lexer))
    return false;

  token_t *varNameTok = lex_current(lexer);

  if (!lex_current_is(lexer, TT_SYMBOL))
    return false;

  if (!lex_can_peek(lexer))
    return false;

  lex_seek(lexer);

  if (lex_at_end(lexer) || lex_current_not_specific_literal(lexer, CLT_EQUALS))
    return false;

  if (!lex_can_peek(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "After a variable assignment must be an expression!");
    return false;
  }

  lex_seek(lexer);

  node_t *value = parser_execute(arena, lexer);
  if (!value)
  {
    S_ERROR(lex_current_cursor(lexer), "After a variable assignment must be a valid expression!");
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

static bool func_arg_name_valid(node_variable_stack_t *vs, string_slice_t *funcName, node_func_definition_args_t *funcArgs, string_slice_t *varName, size_t cursor)
{
  // Checks if the name is the function name.
  if (ss_compare(varName, funcName))
  {
    S_ERROR_SS(cursor, "Variable '" SS_FMT "' can't have the same name as the function definition!", varName);
    return false;
  }

  // Checks if the name is already used in the func args.
  for (size_t i = 0; i < funcArgs->count; ++i)
  {
    if (ss_compare(varName, node_func_args_at(funcArgs, i)))
    {
      S_ERROR_SS(cursor, "Variable '" SS_FMT "' already used in function definition!", varName);
      return false;
    }
  }

  // Checks if the name is in the variable stack or is a buildin constant or function.
  if (!vs_check_variable_name_errors(vs, varName, cursor))
    return false;

  return true;
}

static bool try_parse_func_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (!lex_is_valid(lexer) || !lex_at_start_of_line(lexer))
    return false;

  token_t *varNameTok = lex_current(lexer);

  if (!lex_current_is(lexer, TT_SYMBOL))
    return false;

  if (!lex_can_peek(lexer))
    return false;

  lex_seek(lexer);

  if (lex_at_end(lexer) || lex_current_not_specific_paren(lexer, PT_OPAREN))
    return false;

  if (!lex_can_peek(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected at least one function argument name!");
    return false;
  }

  node_func_definition_args_t funcArgs = {0};
  token_t *lastTok = NULL;

  do
  {
    lex_seek(lexer);

    if (!lastTok && lex_current_is_specific_literal(lexer, CLT_COMMA))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a name!");
      return false;
    }

    if (lastTok && tok_is_specific_literal(lastTok, CLT_COMMA) && lex_current_is_specific_literal(lexer, CLT_COMMA))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a name!");
      return false;
    }

    if (lex_at_end(lexer))
      return false;

    // Parse arg name.
    if (lex_current_is(lexer, TT_SYMBOL))
    {
      token_t *currTok = lex_current(lexer);

      // Check if function argument is already used.
      if (!func_arg_name_valid(vs, &varNameTok->as.symbol, &funcArgs, &currTok->as.symbol, currTok->cursor))
        return false;

      // Add the argument if valid.
      node_func_definition_args_append(arena, &funcArgs, currTok->as.symbol);
    }
    // Continue to next arg name.
    else if (lex_current_is_specific_literal(lexer, CLT_COMMA))
      continue;
    // End arg name parsing.
    else if (lex_current_is_specific_paren(lexer, PT_CPAREN))
      break;
    else
    {
      S_ERROR(lex_current_cursor(lexer), "Unexpected token!");
      return false;
    }

    lastTok = lex_current(lexer);
  } while (!lex_at_end(lexer));

  if (lastTok && tok_is_specific_literal(lastTok, CLT_COMMA))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected a name!");
    return false;
  }

  if (lex_at_end(lexer) || lex_current_not_specific_paren(lexer, PT_CPAREN))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected a closing paren, then an assignment and then an expression!");
    return false;
  }

  if (node_func_args_empty(&funcArgs))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected at least one function argument!");
    return false;
  }

  if (!lex_can_peek(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "Invalid function definition! Expected at least an assignment and then an expression.");
    return false;
  }

  lex_seek(lexer);

  if (lex_at_end(lexer) || lex_current_not_specific_literal(lexer, CLT_EQUALS))
    return false;

  if (!lex_can_peek(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "After a variable assignment must be an expression!");
    return false;
  }

  lex_seek(lexer);

  node_t *expression = parser_execute(arena, lexer);
  if (!expression)
  {
    S_ERROR(lex_current_cursor(lexer), "After a variable assignment must be a valid expression");
    return false;
  }

  // Checks if the variable name is already used
  if (!vs_check_variable_name_errors(vs, &varNameTok->as.symbol, varNameTok->cursor))
    return false;

  // Create and add a new variable.
  node_variable_t funcVar = node_variable_function(varNameTok->as.symbol, funcArgs, expression);
  vs_variable_append(arena, vs, funcVar);
  return true;
}

static inline bool _contains_equals(lexer_t *lexer, token_t *current, size_t pos)
{
  (void)lexer;
  (void)pos;
  return tok_is_specific_literal(current, CLT_EQUALS);
}

bool parse_node_variables(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (!lex_is_valid(lexer))
    return true;

  lex_reset_pos(lexer);

  size_t varCount = 0;

  while (!lex_at_eof(lexer))
  {
    // Not really needed propably.
    if (!lex_contains_in_current_line(lexer, _contains_equals))
    {
      lex_seek_till_end(lexer);

      // Seek to the next line if possible.
      if (!lex_at_eof(lexer))
        lex_seek(lexer);

      continue;
    }

    ++varCount;

    if (try_parse_const_variable(vs, arena, lexer))
    {
      // Seek to the next line if possible.
      if (!lex_at_eof(lexer))
        lex_seek(lexer);

      continue;
    }

    lex_reset_to_start_of_line(lexer);

    if (!try_parse_func_variable(vs, arena, lexer))
      lex_seek_till_end(lexer);

    // Seek to the next line if possible.
    if (!lex_at_eof(lexer))
      lex_seek(lexer);
  }

  lex_reset_pos(lexer);

  return vs->count == varCount;
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
    print_node_ex(vs, variable->expression, indented, 1);
    printf("\n");
    if (indented && i < vs->count - 1)
      printf("\n");
  }
}

static node_t *try_parse_primary(arena_t *arena, lexer_t *lexer, size_t *depth)
{
  node_t *node = NULL;

  if (!node)
    node = try_parse_paren(arena, lexer, depth);

  if (!node)
    node = try_parse_func(arena, lexer, depth);

  if (!node)
    node = try_parse_constant(arena, lexer);

  if (!node)
    node = try_parse_prefix_expression(arena, lexer, depth);

  return node;
}

// TODO: FIX!
static node_t *next_node_by_precedence(arena_t *arena, lexer_t *lexer, size_t *depth, e_precedence precedence)
{
  node_t *node = NULL;

  if (precedence <= BP_EXPONENT)
    node = try_parse_binop(arena, lexer, depth, precedence + 1);

  if (precedence == BP_PRIMARY)
    node = try_parse_primary(arena, lexer, depth);

  if (!lex_at_end(lexer) && !node)
    S_ERROR(lex_current_cursor(lexer), "Invalid token found!");

  return node;
}

static node_t *try_parse_constant(arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lex_at_end(lexer))
    return NULL;

  token_t *token = lex_current(lexer);

  bool found = false;
  node_const_t constant = {0};

  if (lex_current_is(lexer, TT_NUMBER))
  {
    constant = node_constant_value(token->as.number);
    found = true;
  }
  else if (lex_current_is_math_constant(lexer))
  {
    // Checks if the symbol is a valid buildin math constant name.
    e_math_constant_type mc = ss_to_math_constant(&token->as.symbol);
    if (mc == MC_INVALID)
      UNREACHABLE("Math-Constant-Type not implemented!");

    constant = node_constant_buildin_const(mc);
    found = true;
  }
  else if (lex_current_is(lexer, TT_SYMBOL))
  {
    constant = node_constant_variable(token->as.symbol);
    found = true;
  }

  if (!found)
    return NULL;

  lex_seek(lexer);

  return node_constant(arena, token->cursor, constant);
}

static node_t *try_parse_prefix_expression(arena_t *arena, lexer_t *lexer, size_t *depth)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lex_at_end(lexer))
    return NULL;

  if (lex_current_not(lexer, TT_OPERATOR))
    return NULL;

  token_t *token = lex_current(lexer);
  e_node_prefix_type type = NPT_PLUS;

  if (lex_current_is_specific_operator(lexer, OP_ADD))
    type = NPT_PLUS;
  else if (lex_current_is_specific_operator(lexer, OP_SUB))
    type = NPT_MINUS;
  else
  {
    if (token->as.op >= OP_COUNT)
      UNREACHABLE("Invalid operator-type!");

    S_ERROR_EX(lex_current_cursor(lexer), "'%c' is an invalid prefix operator!", operatorTypeIdentifiers[token->as.op]);
    return NULL;
  }

  if (!lex_can_peek(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected an expression after a prefix operator!");
    return NULL;
  }

  lex_seek(lexer);

  if (!lex_at_end(lexer) && lex_current_is(lexer, TT_OPERATOR))
  {
    S_ERROR(lex_current_cursor(lexer), "Nested prefix operators are not allowed!");
    return NULL;
  }

  node_t *child = try_parse_primary(arena, lexer, depth);
  if (!child)
    return NULL;

  return node_prefix_expression(arena, token->cursor, type, child);
}

// static node_t *try_parse_pow_op(arena_t *arena, lexer_t *lexer, size_t *depth)
// {
//   ASSERT_NULL(arena);
//   ASSERT_NULL(lexer);
//   ASSERT_NULL(depth);

//   if (lex_at_end(lexer))
//     return NULL;

//   node_t *lhs = try_parse_primary(arena, lexer, depth);
//   if (!lhs)
//     return NULL;

//   if (lex_at_end(lexer))
//     return lhs;

//   if (lex_current_not(lexer, TT_OPERATOR))
//   {
//     // The specific functions should handle that token.
//     if (*depth != 0)
//       return lhs;

//     S_ERROR(lex_current_cursor(lexer), "Expected an operator!");
//     return NULL;
//   }

//   token_t *opTok = lex_current(lexer);

//   if (lex_current_not_specific_operator(lexer, OP_POW))
//     return lhs;

//   lex_seek(lexer);

//   if (lex_at_end(lexer))
//   {
//     S_ERROR(lex_current_cursor(lexer) - 1, "Expected an expression after an operator!");
//     return NULL;
//   }

//   node_t *rhs = try_parse_primary(arena, lexer, depth);
//   if (!rhs)
//     return NULL;

//   return node_binop(arena, opTok->cursor, opTok->as.op, lhs, rhs);
// }

// static node_t *try_parse_point_op(arena_t *arena, lexer_t *lexer, size_t *depth)
// {
//   ASSERT_NULL(arena);
//   ASSERT_NULL(lexer);
//   ASSERT_NULL(depth);

//   if (lex_at_end(lexer))
//     return NULL;

//   node_t *lhs = try_parse_pow_op(arena, lexer, depth);
//   if (!lhs)
//     return NULL;

//   if (lex_at_end(lexer))
//     return lhs;

//   if (lex_current_not(lexer, TT_OPERATOR))
//   {
//     // The specific functions should handle that token.
//     if (*depth != 0)
//       return lhs;

//     S_ERROR(lex_current_cursor(lexer), "Expected an operator!");
//     return NULL;
//   }

//   token_t *opTok = lex_current(lexer);

//   if (lex_current_not_specific_operator(lexer, OP_MUL) || lex_current_not_specific_operator(lexer, OP_DIV))
//     return lhs;

//   lex_seek(lexer);

//   if (lex_at_end(lexer))
//   {
//     S_ERROR(lex_current_cursor(lexer) - 1, "Expected an expression after an operator!");
//     return NULL;
//   }

//   node_t *rhs = try_parse_pow_op(arena, lexer, depth);
//   if (!rhs)
//     return NULL;

//   return node_binop(arena, opTok->cursor, opTok->as.op, lhs, rhs);
// }

// static node_t *try_parse_line_op(arena_t *arena, lexer_t *lexer, size_t *depth)
// {
//   ASSERT_NULL(arena);
//   ASSERT_NULL(lexer);
//   ASSERT_NULL(depth);

//   if (lex_at_end(lexer))
//     return NULL;

//   node_t *lhs = try_parse_point_op(arena, lexer, depth);
//   if (!lhs)
//     return NULL;

//   if (lex_at_end(lexer))
//     return lhs;

//   if (lex_current_not(lexer, TT_OPERATOR))
//   {
//     // The specific functions should handle that token.
//     if (*depth != 0)
//       return lhs;

//     S_ERROR(lex_current_cursor(lexer), "Expected an operator!");
//     return NULL;
//   }

//   token_t *opTok = lex_current(lexer);

//   if (lex_current_not_specific_operator(lexer, OP_ADD) || lex_current_not_specific_operator(lexer, OP_SUB))
//     return lhs;

//   lex_seek(lexer);

//   if (lex_at_end(lexer))
//   {
//     S_ERROR(lex_current_cursor(lexer) - 1, "Expected an expression after an operator!");
//     return NULL;
//   }

//   node_t *rhs = try_parse_point_op(arena, lexer, depth);
//   if (!rhs)
//     return NULL;

//   return node_binop(arena, opTok->cursor, opTok->as.op, lhs, rhs);
// }

// TODO: Fix right associative ('pow') parsing!
// TODO: Maybe add optimizations like '10*1' to just '10', or '10/1' to just '10'
//       Means that whenever a 1 occours in a point or pow precedence, remove the operator and just return
//       the constant which isn't 1.
static node_t *try_parse_binop(arena_t *arena, lexer_t *lexer, size_t *depth, e_precedence precedence)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(depth);

  if (lex_at_end(lexer))
    return NULL;

  node_t *lhs = next_node_by_precedence(arena, lexer, depth, precedence);
  if (!lhs)
    return NULL;

  if (lex_at_end(lexer))
    return lhs;

  if (lex_current_not(lexer, TT_OPERATOR))
  {
    // The specific functions should handle that token.
    if (*depth != 0)
      return lhs;

    S_ERROR(lex_current_cursor(lexer), "Expected an operator!");
    return NULL;
  }

  token_t *opTok = lex_current(lexer);

  e_operator_type opType = opTok->as.op;

  // TODO: precedence is wrong!
  if (precedence < operatorPrecedence[opType])
  {
    fprintf(stderr, "ERROR(%zu): Invalid precedence for current operator! (OPERATOR: '%s', PRECEDENCE = '%d')\n", lex_current_cursor(lexer), operatorTypeNames[opType], precedence);
    return NULL;
  }

  lex_seek(lexer);

  if (lex_at_end(lexer))
  {
    S_ERROR(lex_current_cursor(lexer) - 1, "Expected an expression after an operator!");
    return NULL;
  }

  node_t *rhs = next_node_by_precedence(arena, lexer, depth, precedence);
  if (!rhs)
    return NULL;

  return node_binop(arena, opTok->cursor, opType, lhs, rhs);
}

static node_t *try_parse_paren(arena_t *arena, lexer_t *lexer, size_t *depth)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(depth);

  if (lex_at_end(lexer))
    return NULL;

  if (!lex_current_is_specific_paren(lexer, PT_OPAREN))
    return NULL;

  size_t tokOParenCursor = lex_current_cursor(lexer);

  if (!lex_can_peek(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected an expression after an open paren!");
    return NULL;
  }

  lex_seek(lexer);

  if (lex_at_end(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected an expression after an open paren!");
    return NULL;
  }

  ++(*depth);
  node_t *child = try_parse_binop(arena, lexer, depth, BP_LINE);
  if (!child)
  {
    S_ERROR(lex_current_cursor(lexer), "Invalid expression!");
    --(*depth);
    return NULL;
  }
  --(*depth);

  if (lex_at_end(lexer) || lex_current_not_specific_paren(lexer, PT_CPAREN))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected a closing paren!");
    return NULL;
  }

  lex_seek(lexer);

  return node_paren(arena, tokOParenCursor, child);
}

static node_t *try_parse_func(arena_t *arena, lexer_t *lexer, size_t *depth)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(depth);

  if (lex_at_end(lexer))
    return NULL;

  if (lex_current_not(lexer, TT_SYMBOL))
    return NULL;

  token_t *funcNameTok = lex_current(lexer);

  if (!lex_can_peek(lexer))
    return NULL;

  lex_seek(lexer);

  // Maybe its just a variable.
  if (lex_at_end(lexer) || lex_current_not_specific_paren(lexer, PT_OPAREN))
  {
    lex_revert_seek(lexer);
    return NULL;
  }

  if (!lex_can_peek(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected at least one function argument!");
    return NULL;
  }

  node_func_args_t argsList = {0};
  token_t *lastTok = NULL;

  do
  {
    lex_seek(lexer);

    if (lex_at_end(lexer))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a valid function!");
      return NULL;
    }

    if (!lastTok && lex_current_is_specific_literal(lexer, CLT_COMMA))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a function argument!");
      return false;
    }

    if (lastTok && tok_is_specific_literal(lastTok, CLT_COMMA) && lex_current_is_specific_literal(lexer, CLT_COMMA))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a function argument!");
      return false;
    }

    if (lastTok && lex_current_is_specific_paren(lexer, PT_CPAREN) && tok_is_specific_literal(lastTok, CLT_COMMA))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a function argument!");
      return false;
    }

    if (!lastTok && lex_current_is_specific_paren(lexer, PT_CPAREN))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a function argument!");
      return NULL;
    }

    ++(*depth);
    node_t *arg = try_parse_binop(arena, lexer, depth, BP_LINE);
    if (!arg)
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a function argument!");
      --(*depth);
      return NULL;
    }
    --(*depth);

    lastTok = lex_current(lexer);

    // Add argument to list.
    node_func_args_append(arena, &argsList, arg);
  } while (!lex_at_end(lexer) && lex_current_not_specific_paren(lexer, PT_CPAREN) && lex_current_is_specific_literal(lexer, CLT_COMMA));

  if (lex_at_end(lexer) || lex_current_not_specific_paren(lexer, PT_CPAREN))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected a closing paren!");
    return NULL;
  }

  node_func_t function = {0};

  // Checks if the symbol is a valid function name.
  e_function_type ft = ss_to_function_type(&funcNameTok->as.symbol);
  if (ft != FT_INVALID)
    function = node_func_buildin(ft, argsList);
  else
    function = node_func_variable(funcNameTok->as.symbol, argsList);

  lex_seek(lexer);

  return node_func(arena, funcNameTok->cursor, function);
}

node_t *parser_execute(arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);

  if (lex_at_end(lexer))
    return NULL;

  size_t depth = 0;
  node_t *root = try_parse_binop(arena, lexer, &depth, BP_LINE);
  if (depth != 0)
    UNREACHABLE("Invalid stack!");

  return root;
}

static double node_constant_to_value(arena_t *arena, node_variable_stack_t *vs, node_t *node, bool *isError)
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

  return result;
}

static node_t *ast_eval_binop(arena_t *arena, node_variable_stack_t *vs, size_t cursor, node_t *lhs, node_t *rhs, e_operator_type op)
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

static node_t *ast_eval_buildin_func(arena_t *arena, node_variable_stack_t *vs, size_t cursor, e_function_type ft, node_func_args_t *args)
{
  bool isError = false;

  switch (ft)
  {
  case FT_SQRT:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(sqrt(value)));
  }
  case FT_EXP:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(exp(value)));
  }
  case FT_SIN:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(sin(value)));
  }
  case FT_ASIN:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(asin(value)));
  }
  case FT_SINH:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(sinh(value)));
  }
  case FT_COS:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(cos(value)));
  }
  case FT_ACOS:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(acos(value)));
  }
  case FT_COSH:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(cosh(value)));
  }
  case FT_TAN:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(tan(value)));
  }
  case FT_ATAN:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(atan(value)));
  }
  case FT_TANH:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(tanh(value)));
  }
  case FT_ATAN2:
  {
    if (args->count != 2)
    {
      E_ERROR_EX(cursor, "%s() takes two arguments!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg1 = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg1)
      return NULL;
    node_t *arg2 = ast_eval_ex(arena, vs, args->items[1]);
    if (!arg2)
      return NULL;
    double value1 = node_constant_to_value(arena, vs, arg1, &isError);
    if (isError)
      return NULL;
    double value2 = node_constant_to_value(arena, vs, arg2, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(atan2(value1, value2)));
  }
  case FT_LN:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(log(value)));
  }
  case FT_LOG10:
  {
    if (args->count != 1)
    {
      E_ERROR_EX(cursor, "%s() takes one argument!", functionTypeIdentifiers[ft]);
      return NULL;
    }
    node_t *arg = ast_eval_ex(arena, vs, args->items[0]);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, &isError);
    if (isError)
      return NULL;
    return node_constant(arena, cursor, node_constant_value(log10(value)));
  }
  case FT_COUNT:
  case FT_INVALID:
  default:
    UNREACHABLE("Invalid function-type!");
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
  case NT_PREFIX_EXPRESSION:
  {
    node_prefix_t *prefix = &expr->as.prefix;

    if (prefix->child && prefix->child->type == NT_PREFIX_EXPRESSION)
    {
      E_ERROR(prefix->child->cursor, "Nested prefix operator found!");
      return NULL;
    }

    node_t *child = ast_eval_ex(arena, vs, prefix->child);
    if (!child)
      return NULL;

    double value = node_constant_to_value(arena, vs, child, &isError);
    if (isError)
      return NULL;

    switch (prefix->type)
    {
    case NPT_PLUS:
    {
      value = 0 + value;
    }
    break;
    case NPT_MINUS:
    {
      value = 0 - value;
    }
    break;
    case NPT_COUNT:
    default:
      UNREACHABLE("Invalid node-prefix-type!");
    }

    return node_constant(arena, expr->cursor, node_constant_value(value));
  }
  case NT_BINOP:
  {
    node_binop_t *binop = &expr->as.binop;

    node_t *lhs = ast_eval_ex(arena, vs, binop->lhs);
    if (!lhs)
      return NULL;

    node_t *rhs = ast_eval_ex(arena, vs, binop->rhs);
    if (!rhs)
      return NULL;

    return ast_eval_binop(arena, vs, expr->cursor, lhs, rhs, binop->type);
  }
  case NT_FUNCTION:
  {
    node_func_t *funcNode = &expr->as.func;

    switch (funcNode->type)
    {
    case FN_BUILDIN_FUNC:
    {
      return ast_eval_buildin_func(arena, vs, expr->cursor, funcNode->as.buildinFunc, &funcNode->args);
    }
    case FN_VARIABLE:
    {
      string_slice_t *funcName = &funcNode->as.variableName;

      node_variable_t *variable = vs_find_variable_ex(vs, funcName, FVM_FUNC);
      if (!variable)
      {
        E_ERROR_SS(expr->cursor, "Variable '" SS_FMT "' not found!", funcName);
        return NULL;
      }

      if (funcNode->args.count != variable->as.funcDef.args.count)
      {
        if (variable->as.funcDef.args.count == 1)
          E_ERROR_SS_2(expr->cursor, SS_FMT "() takes '%zu' argument!", &variable->as.funcDef.name, variable->as.funcDef.args.count);
        else
          E_ERROR_SS_2(expr->cursor, SS_FMT "() takes '%zu' arguments!", &variable->as.funcDef.name, variable->as.funcDef.args.count);
        return NULL;
      }

      for (size_t i = 0; i < funcNode->args.count; ++i)
      {
        // This is how the arg name can be read.
        // string_slice_t *argName = &variable->as.funcDef.args.items[i];

        node_t *arg = ast_eval_ex(arena, vs, funcNode->args.items[i]);
        if (!arg)
          return NULL;

        double value = node_constant_to_value(arena, vs, arg, &isError);
        if (isError)
          return NULL;

        printf("FUNC('" SS_FMT "') -> ARG-%zu = '%f'\n", SS_ARG(&variable->as.funcDef.name), i, value);
      }

      // TODO: Implement!
      // return node_constant(arena, expr->cursor, node_constant_value(log(value)));

      E_ERROR(expr->cursor, "Custom function evaluation not implemented yet!");
      return NULL;
    }
    case FN_COUNT:
    default:
      UNREACHABLE("Invalid func-node-type!");
    }
  }
  case NT_PAREN:
  {
    node_paren_t *parenNode = &expr->as.paren;
    node_t *child = ast_eval_ex(arena, vs, parenNode->child);
    if (!child)
      return NULL;
    return node_constant(arena, expr->cursor, child->as.constant);
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
  case NT_PREFIX_EXPRESSION:
  {
    node_prefix_t *prefixNode = &node->as.prefix;
    node_t *child = prefixNode->child;

    _PRINT_DEPTH_SPACES(indented, depth);

    switch (prefixNode->type)
    {
    case NPT_PLUS:
      printf("prefix_plus(");
      break;
    case NPT_MINUS:
      printf("prefix_minus(");
      break;
    case NPT_COUNT:
    default:
      UNREACHABLE("Invalid node-prefix-type!");
    }

    bool isArgTypeConst = child->type == NT_CONSTANT && child->as.constant.type != CN_VARIABLE;

    if (indented && !isArgTypeConst)
      printf("\n");

    print_node_ex(vs, child, indented, !isArgTypeConst ? depth + 1 : 0);

    if (indented && !isArgTypeConst)
      printf("\n");

    _PRINT_DEPTH_SPACES(indented, !isArgTypeConst ? depth : 0);

    printf(")");
  }
  break;
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
    case CN_BUILDIN_CONST:
    {
      e_math_constant_type mc = node->as.constant.as.buildinConstant;
      if (mc == MC_INVALID)
        UNREACHABLE("Invalid math-constant-type!");

      printf("%s (" DOUBLE_PRINT_FORMAT ", %s)", mathConstantTypeIdentifiers[mc], mathConstantTypeValues[mc], mathConstantTypeNames[mc]);
    }
    break;
    case CN_VARIABLE:
    {
      printf("'" SS_FMT "' =%s", SS_ARG(&node->as.constant.as.variableName), indented ? "\n" : " ");

      node_variable_t *variable = vs_find_variable_ex(vs, &node->as.constant.as.variableName, FVM_CONST);
      if (!variable)
      {
        _PRINT_DEPTH_SPACES(indented, depth + 1);
        printf("[Variable not found]");
        break;
      }

      print_node_ex(vs, variable->expression, indented, depth + 1);
    }
    break;
    case CN_COUNT:
    default:
      UNREACHABLE("Invalid constant-node-type!");
    }
  }
  break;
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
  }
  break;
  case NT_FUNCTION:
  {
    node_func_t *funcNode = &node->as.func;

    _PRINT_DEPTH_SPACES(indented, depth);

    switch (funcNode->type)
    {
    case FN_BUILDIN_FUNC:
    {
      if (funcNode->as.buildinFunc >= FT_COUNT)
        UNREACHABLE("Invalid function-type!");

      printf("%s(", functionTypeIdentifiers[funcNode->as.buildinFunc]);
    }
    break;
    case FN_VARIABLE:
    {
      printf("'" SS_FMT "'", SS_ARG(&funcNode->as.variableName));

      node_variable_t *variable = vs_find_variable_ex(vs, &funcNode->as.variableName, FVM_FUNC);
      if (!variable)
      {
        printf(" - [Variable not found] (");
        break;
      }
      else
        printf("(");
    }
    break;
    case FN_COUNT:
    default:
      UNREACHABLE("Invalid func-node-type!");
    }

    node_func_args_t *args = &funcNode->args;

    if (args->count <= 0)
      UNREACHABLE("No arguments found in function node!");

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
  }
  break;
  case NT_PAREN:
  {
    node_paren_t *parenNode = &node->as.paren;
    node_t *child = parenNode->child;

    _PRINT_DEPTH_SPACES(indented, depth);

    printf("paren(");

    bool isArgTypeConst = child->type == NT_CONSTANT && child->as.constant.type != CN_VARIABLE;

    if (indented && !isArgTypeConst)
      printf("\n");

    print_node_ex(vs, child, indented, !isArgTypeConst ? depth + 1 : 0);

    if (indented && !isArgTypeConst)
      printf("\n");

    _PRINT_DEPTH_SPACES(indented, !isArgTypeConst ? depth : 0);

    printf(")");
  }
  break;
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

  printf("PASSED!\n");
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
    printf("Internal tree:\n");
    printf("\n");
    print_node(vs, test->expressionTree, indented);
    printf("\n");

    printf("Generated tree:\n");
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
