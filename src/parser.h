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
        printf("\033[32m:  \033[0m");        \
  } while (0)

static inline bool _contains_equals(lexer_t *lexer, token_t *current, size_t pos)
{
  (void)lexer;
  (void)pos;
  return tok_is_specific_literal(current, CLT_EQUALS);
}

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

static inline bool node_func_args_empty(node_func_definition_args_t *args)
{
  ASSERT_NULL(args);
  return args->count == 0;
}

static inline string_slice_t *node_func_args_at(node_func_definition_args_t *args, size_t index)
{
  ASSERT_NULL(args);
  assert(index < args->count);
  return &args->items[index];
}

static inline void node_func_definition_args_append(arena_t *arena, node_func_definition_args_t *funcArgs, string_slice_t arg)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(funcArgs);
  arena_da_append(arena, funcArgs, arg);
}

typedef struct
{
  // The name of the function. With this name the function can be called in an expression.
  string_slice_t name;
  // A list of function argument names, which can be used, when using these symbols in the expression.
  node_func_definition_args_t args;
} node_function_definition_t;

typedef struct
{
  string_slice_t funcName;
  string_slice_t argName;
} node_function_arg_definition_t;

typedef union
{
  string_slice_t varDef;
  node_function_definition_t funcDef;
  node_function_arg_definition_t funcArgDef;
} u_node_variable_as;

typedef enum
{
  // Just a constant variable definition. This means it evaluates directly down to a constant value like a math constant.
  NV_CONST_VARIABLE_DEFINITION,
  // A function definition. This means it will have at least one argument, which can be used in the assined expression.
  NV_FUNCTION_DEFINITION,
  // Defines an argument from a function. Stores the function-name and the argument name.
  NV_FUNCTION_ARG_DEFINITION,

  // Count of variable names.
  NV_COUNT
} e_node_variable_type;

static_assert(NV_COUNT == 3, "Amount of variable-node-types have changed");

const char *nodeVariableTypeNames[NV_COUNT] = {
    [NV_CONST_VARIABLE_DEFINITION] = "Constant-Definition",
    [NV_FUNCTION_DEFINITION] = "Function-Definition",
    [NV_FUNCTION_ARG_DEFINITION] = "Function-Argument-Definition",
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
#define node_variable_func_arg(func, arg) \
  ((node_variable_t){.type = NV_FUNCTION_ARG_DEFINITION, .as.funcArgDef.funcName = (func), .as.funcArgDef.argName = (arg), .expression = NULL})

static inline void vs_set_expression(node_variable_t *var, node_t *root)
{
  ASSERT_NULL(var);
  var->expression = root;
}

typedef struct
{
  node_variable_t *items;
  size_t capacity;
  size_t count;
} node_variable_stack_t;

static inline bool vs_empty(node_variable_stack_t *vs)
{
  ASSERT_NULL(vs);

  return vs->count == 0;
}

static inline void vs_variable_push(arena_t *a, node_variable_stack_t *vs, node_variable_t var)
{
  ASSERT_NULL(a);
  ASSERT_NULL(vs);

  // TODO: Rethink! Check for duplicates.
  arena_da_append(a, vs, var);
}

static inline void vs_variable_pop_ex(node_variable_stack_t *vs, size_t count)
{
  ASSERT_NULL(vs);
  vs->count -= count;
}

static inline node_variable_t *vs_variable_pop(node_variable_stack_t *vs)
{
  ASSERT_NULL(vs);
  node_variable_t *var = &vs->items[vs->count--];
  return var;
}

static inline node_variable_t *vs_at(node_variable_stack_t *vs, size_t index)
{
  ASSERT_NULL(vs);

  if (vs_empty(vs))
    return NULL;

  assert(index < vs->count);

  return &(vs)->items[index];
}

typedef enum
{
  FVM_ALL,
  FVM_CONST,
  FVM_FUNC,
  FVM_FUNC_ARG,

  FVM_COUNT
} e_find_var_mode;

static_assert(FVM_COUNT == 4, "Amount of find node var modes have changed!");

typedef struct
{
  string_slice_t **items;
  size_t count;
  size_t capacity;
} vs_call_stack_t;

static inline bool vs_call_stack_name_exists(vs_call_stack_t *stack, string_slice_t *name);
static inline string_slice_t *vs_call_stack_at(vs_call_stack_t *stack, size_t index);
static inline string_slice_t *vs_call_stack_current(vs_call_stack_t *stack);
static inline bool vs_call_stack_can_pop(vs_call_stack_t *stack);
static inline void vs_call_stack_pop(vs_call_stack_t *stack);
static inline void vs_call_stack_push(arena_t *arena, vs_call_stack_t *stack, string_slice_t *name);
static inline void print_vs_call_stack(vs_call_stack_t *stack);

static inline string_slice_t *vs_call_stack_at(vs_call_stack_t *stack, size_t index)
{
  ASSERT_NULL(stack);
  if (stack->count == 0)
    return NULL;
  assert(index < stack->count);

  return stack->items[index];
}

static inline string_slice_t *vs_call_stack_current(vs_call_stack_t *stack)
{
  ASSERT_NULL(stack);

  if (stack->count == 0)
    return NULL;

  string_slice_t *current = stack->items[stack->count - 1];

  // printf("\nDEBUG: Current item in the stack is '" SS_FMT "'\n", SS_ARG(current));

  return current;
}

static inline void vs_call_stack_push(arena_t *arena, vs_call_stack_t *stack, string_slice_t *name)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(stack);
  ASSERT_NULL(name);

  // printf("\nDEBUG: Pushing '" SS_FMT "' to the stack\n", SS_ARG(name));

  arena_da_append(arena, stack, name);
}

static inline bool vs_call_stack_can_pop(vs_call_stack_t *stack)
{
  ASSERT_NULL(stack);
  return stack->count != 0;
}

static inline void vs_call_stack_pop(vs_call_stack_t *stack)
{
  ASSERT_NULL(stack);
  assert(vs_call_stack_can_pop(stack));

  // printf("\nDEBUG: Popping '" SS_FMT "' from the stack\n", SS_ARG(vs_call_stack_at(stack, stack->count - 1)));

  stack->count--;
}

static bool vs_call_stack_name_exists(vs_call_stack_t *stack, string_slice_t *name)
{
  ASSERT_NULL(stack);
  ASSERT_NULL(name);

  for (size_t i = 0; i < stack->count; ++i)
  {
    string_slice_t *curr = vs_call_stack_at(stack, stack->count - 1 - i);

    if (ss_compare(name, curr))
      return true;
  }

  return false;
}

static void print_vs_call_stack(vs_call_stack_t *stack)
{
  ASSERT_NULL(stack);

  printf("Printing Call-Stack:\n");

  if (stack->count == 0)
  {
    printf("\n");
    printf("Call-Stack is empty!\n");
    return;
  }

  for (size_t i = 0; i < stack->count; ++i)
  {
    string_slice_t *curr = vs_call_stack_at(stack, i);

    printf("STACK(%zu) = '" SS_FMT "'\n", i, SS_ARG(curr));
  }
}

static node_variable_t *vs_find_variable_func_arg(node_variable_stack_t *vs, string_slice_t *name, vs_call_stack_t *callStack)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(name);
  ASSERT_NULL(callStack);

  node_variable_t *curr = NULL;

  for (size_t csi = 0; csi < callStack->count; ++csi)
  {
    string_slice_t *currItem = vs_call_stack_at(callStack, csi);

    for (size_t vsi = 0; vsi < vs->count; ++vsi)
    {
      curr = vs_at(vs, vsi);

      if (curr->type != NV_FUNCTION_ARG_DEFINITION)
        continue;

      if (ss_compare(currItem, &curr->as.funcArgDef.funcName))
        return curr;
    }
  }

  return NULL;
}

static node_variable_t *vs_find_variable_ex(node_variable_stack_t *vs, string_slice_t *name, string_slice_t *funcName, e_find_var_mode mode)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(name);
  assert(mode < FVM_COUNT);

  node_variable_t *curr = NULL;

  for (size_t i = 0; i < vs->count; ++i)
  {
    curr = vs_at(vs, i);

    switch (mode)
    {
    case FVM_ALL:
    {
      switch (curr->type)
      {
      case NV_CONST_VARIABLE_DEFINITION:
      {
        if (ss_compare(name, &curr->as.varDef))
          return curr;
      }
      break;
      case NV_FUNCTION_DEFINITION:
      {
        if (ss_compare(name, &curr->as.funcDef.name))
          return curr;
      }
      break;
      case NV_FUNCTION_ARG_DEFINITION:
      {
        if (!funcName)
          continue;

        if (ss_compare(funcName, &curr->as.funcArgDef.funcName) &&
            ss_compare(name, &curr->as.funcArgDef.argName))
          return curr;
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
        if (ss_compare(name, &curr->as.varDef))
          return curr;
    }
    break;
    case FVM_FUNC:
    {
      if (curr->type == NV_FUNCTION_DEFINITION)
        if (ss_compare(name, &curr->as.funcDef.name))
          return curr;
    }
    break;
    case FVM_FUNC_ARG:
    {
      if (!funcName)
        continue;

      if (curr->type == NV_FUNCTION_ARG_DEFINITION)
        if (ss_compare(funcName, &curr->as.funcArgDef.funcName) &&
            ss_compare(name, &curr->as.funcArgDef.argName))
          return curr;
    }
    break;
    case FVM_COUNT:
    default:
      UNREACHABLE("Invalid find-var-mode!");
    }
  }

  return NULL;
}

#define vs_find_variable(vs, name, func) \
  vs_find_variable_ex((vs), (name), (func), FVM_ALL)

bool vs_variable_name_exists_ex(node_variable_stack_t *vs, string_slice_t *name, string_slice_t *funcName, e_find_var_mode mode)
{
  node_variable_t *v = vs_find_variable_ex(vs, name, funcName, mode);
  return v != NULL;
}

#define vs_variable_name_exists(vs, name, func) \
  vs_variable_name_exists_ex((vs), (name), (func), FVM_ALL)

static inline bool vs_check_variable_name_errors(node_variable_stack_t *vs, string_slice_t *name, string_slice_t *funcName, size_t cursor)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(name);

  if (vs_variable_name_exists(vs, name, funcName))
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

static inline string_slice_t *vs_get_variable_name(node_variable_t *variable)
{
  ASSERT_NULL(variable);

  switch (variable->type)
  {
  case NV_CONST_VARIABLE_DEFINITION:
    return &variable->as.varDef;
  case NV_FUNCTION_DEFINITION:
    return &variable->as.funcDef.name;
  case NV_FUNCTION_ARG_DEFINITION:
    return &variable->as.funcArgDef.argName;
  case NV_COUNT:
  default:
    UNREACHABLE("Invalid node-variable-type!");
  }
}

void vs_set_func_arg_expression(node_variable_stack_t *vs, string_slice_t *funcName, string_slice_t *argName, node_t *expression)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(funcName);
  ASSERT_NULL(argName);

  node_variable_t *argVar = vs_find_variable_ex(vs, argName, funcName, FVM_FUNC_ARG);
  assert(argVar);

  vs_set_expression(argVar, expression);
}

void vs_set_func_args(node_variable_stack_t *vs, node_function_definition_t *varFuncDef, node_func_args_t *funcArgs)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(varFuncDef);
  ASSERT_NULL(funcArgs);
  assert(varFuncDef->args.count == funcArgs->count);

  for (size_t i = 0; i < funcArgs->count; ++i)
    vs_set_func_arg_expression(vs, &varFuncDef->name, &varFuncDef->args.items[i], funcArgs->items[i]);
}

void vs_reset_func_args(node_variable_stack_t *vs, node_function_definition_t *varFuncDef)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(varFuncDef);

  for (size_t i = 0; i < varFuncDef->args.count; ++i)
    vs_set_func_arg_expression(vs, &varFuncDef->name, &varFuncDef->args.items[i], NULL);
}

static bool try_parse_const_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer, bool *isError);
static bool try_parse_func_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer, bool *isError);
void print_variables(arena_t *arena, node_variable_stack_t *vs, bool showValue, bool indented);

static node_t *try_parse_constant(arena_t *arena, lexer_t *lexer);
static node_t *try_parse_prefix_expression(arena_t *arena, lexer_t *lexer, size_t *stackSize, bool *isError);
static node_t *try_parse_paren(arena_t *arena, lexer_t *lexer, size_t *stackSize, bool *isError);
static node_t *try_parse_func(arena_t *arena, lexer_t *lexer, size_t *stackSize, bool *isError);
static node_t *try_parse_primary(arena_t *arena, lexer_t *lexer, size_t *stackSize, bool *isError);
static node_t *try_parse_binop(arena_t *arena, lexer_t *lexer, size_t *stackSize, e_precedence precedence, bool *isError);

static void print_node_ex(arena_t *arena, node_variable_stack_t *vs, node_t *node, bool indented, size_t depth, vs_call_stack_t *callStack);
void print_node(arena_t *arena, node_variable_stack_t *vs, node_t *node, bool indented);
void print_node_simple(arena_t *arena, node_t *node, bool indented);

node_t *ast_eval_ex(arena_t *arena, node_variable_stack_t *vs, node_t *expr, vs_call_stack_t *callStack);

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

static bool try_parse_const_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer, bool *isError)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(isError);
  assert(lex_is_valid(lexer));

  if (*isError || !lex_at_start_of_line(lexer))
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
    *isError = true;
    return false;
  }

  lex_seek(lexer);

  if (lex_at_end(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected a valid expression!");
    *isError = true;
    return false;
  }

  size_t stackSize = 0;
  node_t *expression = try_parse_binop(arena, lexer, &stackSize, BP_LINE, isError);
  assert(stackSize == 0);

  if (!expression || *isError)
  {
    S_ERROR(lex_current_cursor(lexer), "After a variable assignment must be a valid expression!");
    *isError = true;
    return false;
  }

  // Checks if the variable name is already used
  if (!vs_check_variable_name_errors(vs, &varNameTok->as.symbol, NULL, varNameTok->cursor))
  {
    *isError = true;
    return false;
  }

  // Create and add a new variable.
  node_variable_t constantVar = node_variable_constant(varNameTok->as.symbol, expression);
  vs_variable_push(arena, vs, constantVar);
  return true;
}

static bool func_arg_name_valid(node_variable_stack_t *vs, string_slice_t *funcName, node_func_definition_args_t *funcArgs, string_slice_t *varName, size_t cursor)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(funcName);
  ASSERT_NULL(funcArgs);
  ASSERT_NULL(varName);

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
  if (!vs_check_variable_name_errors(vs, varName, NULL, cursor))
    return false;

  return true;
}

static bool try_parse_func_variable(node_variable_stack_t *vs, arena_t *arena, lexer_t *lexer, bool *isError)
{
  ASSERT_NULL(vs);
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(isError);
  assert(lex_is_valid(lexer));

  if (*isError || !lex_at_start_of_line(lexer))
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
    *isError = true;
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
      *isError = true;
      return false;
    }

    if (lastTok && tok_is_specific_literal(lastTok, CLT_COMMA) && lex_current_is_specific_literal(lexer, CLT_COMMA))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a name!");
      *isError = true;
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
      // TODO: Rethink!
      // Probably not a function var defintion, but a function call.
      // S_ERROR(lex_current_cursor(lexer), "Unexpected token!");
      // *isError = true;
      return false;
    }

    lastTok = lex_current(lexer);
  } while (!lex_at_end(lexer));

  if (lastTok && tok_is_specific_literal(lastTok, CLT_COMMA))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected a name!");
    *isError = true;
    return false;
  }

  if (lex_at_end(lexer) || lex_current_not_specific_paren(lexer, PT_CPAREN))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected a closing paren, then an assignment and then an expression!");
    *isError = true;
    return false;
  }

  if (node_func_args_empty(&funcArgs))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected at least one function argument!");
    *isError = true;
    return false;
  }

  if (!lex_can_peek(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "Invalid function definition! Expected at least an assignment and then an expression.");
    *isError = true;
    return false;
  }

  lex_seek(lexer);

  if (lex_at_end(lexer) || lex_current_not_specific_literal(lexer, CLT_EQUALS))
    return false;

  if (!lex_can_peek(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "After a variable assignment must be an expression!");
    *isError = true;
    return false;
  }

  lex_seek(lexer);

  if (lex_at_end(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected a valid expression!");
    *isError = true;
    return false;
  }

  size_t stackSize = 0;
  node_t *expression = try_parse_binop(arena, lexer, &stackSize, BP_LINE, isError);
  assert(stackSize == 0);

  if (!expression || *isError)
  {
    S_ERROR(lex_current_cursor(lexer), "After a variable assignment must be a valid expression");
    *isError = true;
    return false;
  }

  // Checks if the variable name is already used
  if (!vs_check_variable_name_errors(vs, &varNameTok->as.symbol, NULL, varNameTok->cursor))
  {
    *isError = true;
    return false;
  }

  // Create and add the function variable.
  node_variable_t funcVar = node_variable_function(varNameTok->as.symbol, funcArgs, expression);
  vs_variable_push(arena, vs, funcVar);

  // Create the function argument links.
  for (size_t i = 0; i < funcArgs.count; ++i)
    vs_variable_push(arena, vs, node_variable_func_arg(varNameTok->as.symbol, funcArgs.items[i]));

  return true;
}

void print_variables(arena_t *arena, node_variable_stack_t *vs, bool showValue, bool indented)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(vs);

  if (vs_empty(vs))
  {
    printf("No variables found.\n");
    return;
  }

  vs_call_stack_t callStack = {0};

  printf("Printing variables (%zu):\n", vs->count);

  for (size_t i = 0; i < vs->count; ++i)
  {
    node_variable_t *variable = vs_at(vs, i);

    switch (variable->type)
    {
    case NV_CONST_VARIABLE_DEFINITION:
    {
      printf("\033[35m" SS_FMT "\033[0m", SS_ARG(&variable->as.varDef));

      vs_call_stack_push(arena, &callStack, &variable->as.varDef);
    }
    break;
    case NV_FUNCTION_DEFINITION:
    {
      printf("\033[35m" SS_FMT "\033[0m", SS_ARG(&variable->as.funcDef.name));
      printf("(");
      for (size_t i = 0; i < variable->as.funcDef.args.count; ++i)
      {
        printf("\033[34m" SS_FMT "\033[0m", SS_ARG(&variable->as.funcDef.args.items[i]));
        if (i < variable->as.funcDef.args.count - 1)
          printf(", ");
      }
      printf(")");

      vs_call_stack_push(arena, &callStack, &variable->as.funcDef.name);
    }
    break;
    case NV_FUNCTION_ARG_DEFINITION:
    {
      printf("\033[35m" SS_FMT "\033[0m"
             "::"
             "\033[34m" SS_FMT "\033[0m",
             SS_ARG(&variable->as.funcArgDef.funcName),
             SS_ARG(&variable->as.funcArgDef.argName));
    }
    break;
    case NV_COUNT:
    default:
      UNREACHABLE("Invalid variable-type!");
      break;
    }

    if (!showValue || !variable->expression)
    {
      printf("\n\n");
      continue;
    }

    printf(" ->%s", indented ? "\n" : " ");
    print_node_ex(arena, vs, variable->expression, indented, 1, &callStack);
    if (vs_call_stack_can_pop(&callStack))
      vs_call_stack_pop(&callStack);

    assert(callStack.count == 0);

    printf("\n");
    if (indented && i < vs->count - 1)
      printf("\n");
  }
}

static node_t *try_parse_primary(arena_t *arena, lexer_t *lexer, size_t *stackSize, bool *isError)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(stackSize);
  ASSERT_NULL(isError);
  assert(lex_is_valid(lexer));

  if (*isError)
    return NULL;

  node_t *node = NULL;

  if (!node)
    node = try_parse_paren(arena, lexer, stackSize, isError);

  if (!node && *isError)
    return NULL;

  if (!node)
    node = try_parse_func(arena, lexer, stackSize, isError);

  if (!node && *isError)
    return NULL;

  if (!node)
    node = try_parse_prefix_expression(arena, lexer, stackSize, isError);

  if (!node && *isError)
    return NULL;

  if (!node)
    node = try_parse_constant(arena, lexer);

  return node;
}

// TODO: FIX!
static node_t *next_node_by_precedence(arena_t *arena, lexer_t *lexer, size_t *stackSize, e_precedence precedence, bool *isError)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(stackSize);
  ASSERT_NULL(isError);
  assert(lex_is_valid(lexer));

  if (*isError)
    return NULL;

  node_t *node = NULL;

  if (!node && precedence <= BP_EXPONENT)
    node = try_parse_binop(arena, lexer, stackSize, precedence + 1, isError);

  if (!node && *isError)
    return NULL;

  if (!node && precedence == BP_PRIMARY)
    node = try_parse_primary(arena, lexer, stackSize, isError);

  if (!node && *isError)
    return NULL;

  // if (!node && !lex_at_end(lexer))
  //   S_ERROR(lex_current_cursor(lexer), "Invalid token found!");

  return node;
}

static node_t *try_parse_constant(arena_t *arena, lexer_t *lexer)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  assert(lex_is_valid(lexer));

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

static node_t *try_parse_prefix_expression(arena_t *arena, lexer_t *lexer, size_t *stackSize, bool *isError)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(stackSize);
  ASSERT_NULL(isError);
  assert(lex_is_valid(lexer));

  if (*isError)
    return NULL;

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
    *isError = true;
    return NULL;
  }

  if (!lex_can_peek(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected an expression after a prefix operator!");
    *isError = true;
    return NULL;
  }

  lex_seek(lexer);

  if (!lex_at_end(lexer) && lex_current_is(lexer, TT_OPERATOR))
  {
    S_ERROR(lex_current_cursor(lexer), "Nested prefix operators are not allowed!");
    *isError = true;
    return NULL;
  }

  node_t *child = try_parse_primary(arena, lexer, stackSize, isError);

  if (!child || *isError)
    return NULL;

  return node_prefix_expression(arena, token->cursor, type, child);
}

// TODO: Fix right associative ('pow') and precedence parsing!
// TODO: Maybe add optimizations like '10*1' to just '10', or '10/1' to just '10'
//       Means that whenever a 1 occours in a point or pow precedence, remove the operator and just return
//       the constant which isn't 1.
static node_t *try_parse_binop(arena_t *arena, lexer_t *lexer, size_t *stackSize, e_precedence precedence, bool *isError)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(stackSize);
  ASSERT_NULL(isError);
  assert(lex_is_valid(lexer));

  if (*isError)
    return NULL;

  if (lex_at_end(lexer))
    return NULL;

  node_t *lhs = next_node_by_precedence(arena, lexer, stackSize, precedence, isError);

  if (!lhs || *isError)
    return NULL;

  if (lex_at_end(lexer))
    return lhs;

  if (lex_current_not(lexer, TT_OPERATOR))
  {
    // The specific functions should handle that token.
    if (*stackSize != 0)
      return lhs;

    S_ERROR(lex_current_cursor(lexer), "Expected an operator!");
    *isError = true;
    return NULL;
  }

  token_t *opTok = lex_current(lexer);

  e_operator_type opType = opTok->as.op;

  // TODO: precedence is wrong!
  if (precedence < operatorPrecedence[opType])
  {
    fprintf(stderr, "ERROR(%zu): Invalid precedence for current operator! (OPERATOR: '%s', PRECEDENCE = '%d')\n", lex_current_cursor(lexer), operatorTypeNames[opType], precedence);
    *isError = true;
    return NULL;
  }

  lex_seek(lexer);

  if (lex_at_end(lexer))
  {
    S_ERROR(lex_current_cursor(lexer) - 1, "Expected an expression after an operator!");
    *isError = true;
    return NULL;
  }

  node_t *rhs = next_node_by_precedence(arena, lexer, stackSize, precedence, isError);

  if (!rhs || *isError)
    return NULL;

  return node_binop(arena, opTok->cursor, opType, lhs, rhs);
}

static node_t *try_parse_paren(arena_t *arena, lexer_t *lexer, size_t *stackSize, bool *isError)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(stackSize);
  ASSERT_NULL(isError);
  assert(lex_is_valid(lexer));

  if (*isError)
    return NULL;

  if (lex_at_end(lexer))
    return NULL;

  if (!lex_current_is_specific_paren(lexer, PT_OPAREN))
    return NULL;

  size_t tokOParenCursor = lex_current_cursor(lexer);

  if (!lex_can_peek(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected an expression after an open paren!");
    *isError = true;
    return NULL;
  }

  lex_seek(lexer);

  if (lex_at_end(lexer))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected an expression after an open paren!");
    *isError = true;
    return NULL;
  }

  ++(*stackSize);
  node_t *child = try_parse_binop(arena, lexer, stackSize, BP_LINE, isError);
  if (!child)
  {
    *isError = true;
    --(*stackSize);
    S_ERROR(lex_current_cursor(lexer), "Expected a valid expression!");
    return NULL;
  }
  --(*stackSize);

  if (lex_at_end(lexer) || lex_current_not_specific_paren(lexer, PT_CPAREN))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected a closing paren!");
    *isError = true;
    return NULL;
  }

  lex_seek(lexer);

  return node_paren(arena, tokOParenCursor, child);
}

static node_t *try_parse_func(arena_t *arena, lexer_t *lexer, size_t *stackSize, bool *isError)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(stackSize);
  ASSERT_NULL(isError);
  assert(lex_is_valid(lexer));

  if (*isError)
    return NULL;

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
    *isError = true;
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
      *isError = true;
      return NULL;
    }

    if (!lastTok && lex_current_is_specific_literal(lexer, CLT_COMMA))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a function argument!");
      *isError = true;
      return NULL;
    }

    if (lastTok && tok_is_specific_literal(lastTok, CLT_COMMA) && lex_current_is_specific_literal(lexer, CLT_COMMA))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a function argument!");
      *isError = true;
      return NULL;
    }

    if (lastTok && lex_current_is_specific_paren(lexer, PT_CPAREN) && tok_is_specific_literal(lastTok, CLT_COMMA))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a function argument!");
      *isError = true;
      return NULL;
    }

    if (!lastTok && lex_current_is_specific_paren(lexer, PT_CPAREN))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected a function argument!");
      *isError = true;
      return NULL;
    }

    ++(*stackSize);
    node_t *arg = try_parse_binop(arena, lexer, stackSize, BP_LINE, isError);
    if (!arg)
    {
      *isError = true;
      --(*stackSize);
      S_ERROR(lex_current_cursor(lexer), "Expected a valid expression as an argument!");
      return NULL;
    }
    --(*stackSize);

    lastTok = lex_current(lexer);

    // Add argument to list.
    node_func_args_append(arena, &argsList, arg);
  } while (!lex_at_end(lexer) && lex_current_not_specific_paren(lexer, PT_CPAREN) && lex_current_is_specific_literal(lexer, CLT_COMMA));

  if (lex_at_end(lexer) || lex_current_not_specific_paren(lexer, PT_CPAREN))
  {
    S_ERROR(lex_current_cursor(lexer), "Expected a closing paren!");
    *isError = true;
    return NULL;
  }

  // Checks if the symbol is a valid function name.
  node_func_t function = {0};
  e_function_type funcType = ss_to_function_type(&funcNameTok->as.symbol);
  if (funcType != FT_INVALID)
    function = node_func_buildin(funcType, argsList);
  else
    function = node_func_variable(funcNameTok->as.symbol, argsList);

  lex_seek(lexer);

  return node_func(arena, funcNameTok->cursor, function);
}

// Example input: "10 * 5 + PI"
node_t *try_parse_expression_without_equals(arena_t *arena, lexer_t *lexer, bool *isError)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(isError);
  assert(lex_is_valid(lexer));

  if (*isError)
    return NULL;

  if (lex_at_end(lexer))
    return NULL;

  size_t stackSize = 0;
  node_t *root = try_parse_binop(arena, lexer, &stackSize, BP_LINE, isError);
  assert(stackSize == 0);

  if (*isError)
    return NULL;

  return root;
}

node_t *try_parse_expression_with_equals(arena_t *arena, lexer_t *lexer, bool *isError)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(isError);
  assert(lex_is_valid(lexer));

  if (*isError)
    return false;

  if (lex_at_end(lexer))
    return false;

  // TODO: Rethink! Maybe throw error when not at the start of the line.
  if (lex_at_start_of_line(lexer))
  {
    if (!lex_current_is_specific_literal(lexer, CLT_EQUALS))
    {
      S_ERROR(lex_current_cursor(lexer), "First token must be an equals sign!");
      *isError = true;
      return false;
    }

    if (!lex_can_peek(lexer))
    {
      S_ERROR(lex_current_cursor(lexer), "Expected an expression after the equals sign!");
      *isError = true;
      return false;
    }

    lex_seek(lexer);
  }

  node_t *root = try_parse_expression_without_equals(arena, lexer, isError);

  if (*isError)
    return NULL;

  return root;
}

typedef struct
{
  node_t **items;
  size_t count;
  size_t capacity;
} node_expressions_list_t;

static inline node_t *node_exprs_at(node_expressions_list_t *exprs, size_t index)
{
  ASSERT_NULL(exprs);
  assert(index < exprs->count);

  return exprs->items[index];
}

static inline void node_exprs_add(arena_t *arena, node_expressions_list_t *result, node_t *node)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(result);
  ASSERT_NULL(node);

  arena_da_append(arena, result, node);
}

// Example input: "= 10 * 5 + PI"
bool try_parse_expression(node_expressions_list_t *exprs, arena_t *arena, lexer_t *lexer, bool *isError, bool usePrefixEqualsSign)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(exprs);
  ASSERT_NULL(isError);
  assert(lex_is_valid(lexer));

  if (*isError)
    return false;

  if (lex_at_end(lexer))
    return false;

  node_t *root = NULL;

  if (usePrefixEqualsSign)
    root = try_parse_expression_with_equals(arena, lexer, isError);
  else
    root = try_parse_expression_without_equals(arena, lexer, isError);

  if (!root || *isError)
    return false;

  node_exprs_add(arena, exprs, root);

  return true;
}

typedef struct
{
  node_variable_stack_t vs;
  node_expressions_list_t exprs;
} parser_t;

static inline bool parser_contains_variables(parser_t *parser)
{
  ASSERT_NULL(parser);

  return parser->vs.count > 0;
}

static inline bool parser_contains_expressions(parser_t *parser)
{
  ASSERT_NULL(parser);

  return parser->exprs.count > 0;
}

parser_t parser_execute_ex(arena_t *arena, lexer_t *lexer, node_variable_stack_t *vs, bool usePrefixEqualsSign)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  ASSERT_NULL(vs);
  assert(lex_is_valid(lexer));

  node_expressions_list_t exprs = {0};

  lex_reset_pos(lexer);

  while (!lex_at_eof(lexer))
  {
    bool isError = false;

    if (!try_parse_const_variable(vs, arena, lexer, &isError))
    {
      if (!isError)
      {
        lex_reset_to_start_of_line(lexer);

        if (!try_parse_func_variable(vs, arena, lexer, &isError))
        {
          if (!isError)
          {
            lex_reset_to_start_of_line(lexer);

            if (!try_parse_expression(&exprs, arena, lexer, &isError, usePrefixEqualsSign))
            {
              // Unhandled tokens or new-lines will be handled here.
              lex_seek_till_end(lexer);
            }
          }
          else
            lex_seek_till_end(lexer);
        }
      }
      else
        lex_seek_till_end(lexer);
    }

    // Seek to the next line if possible.
    if (!lex_at_eof(lexer))
      lex_seek(lexer);
  }

  assert(lex_at_eof(lexer));

  return (parser_t){
      .vs = *vs,
      .exprs = exprs,
  };
}

parser_t parser_execute(arena_t *arena, lexer_t *lexer, bool usePrefixEqualsSign)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(lexer);
  assert(lex_is_valid(lexer));

  node_variable_stack_t vs = {0};

  return parser_execute_ex(arena, lexer, &vs, usePrefixEqualsSign);
}

static double node_constant_to_value(arena_t *arena, node_variable_stack_t *vs, node_t *node, vs_call_stack_t *callStack, bool *isError)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(vs);
  ASSERT_NULL(node);
  ASSERT_NULL(callStack);
  ASSERT_NULL(isError);
  assert(node->type == NT_CONSTANT);

  if (*isError)
    return 0;

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
      UNREACHABLE("Invalid math-constant-type!");

    result = mathConstantTypeValues[mc];
  }
  break;
  case CN_VARIABLE:
  {
    string_slice_t *varName = &constant->as.variableName;

    // Check if the variable is just a constant.
    node_variable_t *variable = vs_find_variable_ex(vs, varName, NULL, FVM_CONST);

    // Check if the variable is a local function argument.
    if (!variable)
    {
      // TODO: This could be the failing recursion problem.
      // variable = vs_find_variable_ex(vs, varName, vs_call_stack_current(callStack), FVM_FUNC_ARG);
      variable = vs_find_variable_func_arg(vs, varName, callStack);
    }

    if (!variable)
    {
      *isError = true;
      E_ERROR_SS(node->cursor, "Variable '" SS_FMT "' not found!", varName);
      return 0;
    }

    if (vs_call_stack_name_exists(callStack, varName))
    {
      *isError = true;
      E_ERROR_SS(node->cursor, "Recursion in '" SS_FMT "' detected!", varName);
      return 0;
    }

    vs_call_stack_push(arena, callStack, varName);
    node_t *resNode = ast_eval_ex(arena, vs, variable->expression, callStack);
    vs_call_stack_pop(callStack);

    if (!resNode)
    {
      *isError = true;
      return 0;
    }

    bool resError = false;
    result = node_constant_to_value(arena, vs, resNode, callStack, &resError);
    if (resError)
    {
      *isError = true;
      return 0;
    }
  }
  break;
  case CN_COUNT:
  default:
    UNREACHABLE("Invalid constant node type!");
  }

  return result;
}

static node_t *ast_eval_binop(arena_t *arena, node_variable_stack_t *vs, size_t cursor, node_t *lhs, node_t *rhs, e_operator_type op, vs_call_stack_t *callStack)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(vs);
  ASSERT_NULL(lhs);
  ASSERT_NULL(rhs);
  ASSERT_NULL(callStack);

  bool isError = false;

  double left = node_constant_to_value(arena, vs, lhs, callStack, &isError);
  if (isError)
    return NULL;

  double right = node_constant_to_value(arena, vs, rhs, callStack, &isError);
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

static node_t *ast_eval_buildin_func(arena_t *arena, node_variable_stack_t *vs, size_t cursor, e_function_type ft, node_func_args_t *args, vs_call_stack_t *callStack)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(vs);
  ASSERT_NULL(args);
  ASSERT_NULL(callStack);

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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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
    node_t *arg1 = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg1)
      return NULL;
    node_t *arg2 = ast_eval_ex(arena, vs, args->items[1], callStack);
    if (!arg2)
      return NULL;
    double value1 = node_constant_to_value(arena, vs, arg1, callStack, &isError);
    if (isError)
      return NULL;
    double value2 = node_constant_to_value(arena, vs, arg2, callStack, &isError);
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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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
    node_t *arg = ast_eval_ex(arena, vs, args->items[0], callStack);
    if (!arg)
      return NULL;
    double value = node_constant_to_value(arena, vs, arg, callStack, &isError);
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

node_t *ast_eval_ex(arena_t *arena, node_variable_stack_t *vs, node_t *expr, vs_call_stack_t *callStack)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(vs);
  ASSERT_NULL(expr);
  ASSERT_NULL(callStack);

  bool isError = false;

  switch (expr->type)
  {
  case NT_CONSTANT:
  {
    // Will evaluate down to a constant value type.
    double value = node_constant_to_value(arena, vs, expr, callStack, &isError);
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

    node_t *child = ast_eval_ex(arena, vs, prefix->child, callStack);
    if (!child)
      return NULL;

    double value = node_constant_to_value(arena, vs, child, callStack, &isError);
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

    node_t *lhs = ast_eval_ex(arena, vs, binop->lhs, callStack);
    if (!lhs)
      return NULL;

    node_t *rhs = ast_eval_ex(arena, vs, binop->rhs, callStack);
    if (!rhs)
      return NULL;

    return ast_eval_binop(arena, vs, expr->cursor, lhs, rhs, binop->type, callStack);
  }
  case NT_FUNCTION:
  {
    node_func_t *funcNode = &expr->as.func;

    switch (funcNode->type)
    {
    case FN_BUILDIN_FUNC:
    {
      return ast_eval_buildin_func(arena, vs, expr->cursor, funcNode->as.buildinFunc, &funcNode->args, callStack);
    }
    case FN_VARIABLE:
    {
      string_slice_t *funcName = &funcNode->as.variableName;

      // Try to get the variable by the name.
      node_variable_t *variable = vs_find_variable_ex(vs, funcName, NULL, FVM_FUNC);
      if (!variable)
      {
        E_ERROR_SS(expr->cursor, "Variable '" SS_FMT "' not found!", funcName);
        return NULL;
      }

      node_function_definition_t *varFuncDef = &variable->as.funcDef;

      // Check if the argument count is equal.
      if (funcNode->args.count != varFuncDef->args.count)
      {
        if (varFuncDef->args.count == 1)
          E_ERROR_SS_2(expr->cursor, SS_FMT "() takes '%zu' argument!", &varFuncDef->name, varFuncDef->args.count);
        else
          E_ERROR_SS_2(expr->cursor, SS_FMT "() takes '%zu' arguments!", &varFuncDef->name, varFuncDef->args.count);
        return NULL;
      }

      // TODO: FIX! Recursion detection does currently sometimes not work correctly when evaluating.
      if (vs_call_stack_name_exists(callStack, funcName))
      {
        E_ERROR_SS(expr->cursor, "Recursion in '" SS_FMT "' detected!", funcName);
        return NULL;
      }

      vs_set_func_args(vs, varFuncDef, &funcNode->args);

      vs_call_stack_push(arena, callStack, funcName);
      node_t *resultNode = ast_eval_ex(arena, vs, variable->expression, callStack);
      vs_call_stack_pop(callStack);

      vs_reset_func_args(vs, varFuncDef);

      if (!resultNode)
        return NULL;

      double result = node_constant_to_value(arena, vs, resultNode, callStack, &isError);
      if (isError)
        return NULL;

      return node_constant(arena, expr->cursor, node_constant_value(result));
    }
    case FN_COUNT:
    default:
      UNREACHABLE("Invalid func-node-type!");
    }
  }
  case NT_PAREN:
  {
    node_paren_t *parenNode = &expr->as.paren;
    node_t *child = ast_eval_ex(arena, vs, parenNode->child, callStack);
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

  vs_call_stack_t callStack = {0};
  node_t *evaluatedNode = ast_eval_ex(arena, vs, expr, &callStack);
  assert(callStack.count == 0);

  if (!evaluatedNode)
    return ast_eval_res_error();

  bool isError = false;
  double result = node_constant_to_value(arena, vs, evaluatedNode, &callStack, &isError);
  if (isError)
    return ast_eval_res_error();

  return ast_eval_res_success(result);
}

void node_print_result(arena_t *arena, node_variable_stack_t *vs, node_t *node)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(vs);
  ASSERT_NULL(node);

  ast_eval_res_t evalRes = ast_eval(arena, vs, node);

  if (evalRes.isError)
    printf(">>> \033[31mERROR\033[0m\n");
  else
    printf(">>> " DOUBLE_PRINT_FORMAT "\n", evalRes.value);
}

void parser_eval(arena_t *arena, parser_t *parser)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(parser);

  if (parser->exprs.count == 0)
  {
    printf("No expressions found to evaluate.\n");
    return;
  }

  printf("Results:\n");

  for (size_t i = 0; i < parser->exprs.count; ++i)
  {
    ast_eval_res_t evalRes = ast_eval(arena, &parser->vs, node_exprs_at(&parser->exprs, i));

    if (evalRes.isError)
      printf("\033[30mEXPR-%zu\033[0m >>> \033[31mERROR\033[0m\n", i + 1);
    else
      printf("\033[30mEXPR-%zu\033[0m >>> " DOUBLE_PRINT_FORMAT "\n", i + 1, evalRes.value);
  }
}

static void print_node_ex(arena_t *arena, node_variable_stack_t *vs, node_t *node, bool indented, size_t depth, vs_call_stack_t *callStack)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(vs);
  ASSERT_NULL(node);
  ASSERT_NULL(callStack);

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

    print_node_ex(arena, vs, child, indented, !isArgTypeConst ? depth + 1 : 0, callStack);

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
      printf("\033[33m" DOUBLE_PRINT_FORMAT "\033[0m", node->as.constant.as.value);
    }
    break;
    case CN_BUILDIN_CONST:
    {
      e_math_constant_type mc = node->as.constant.as.buildinConstant;
      if (mc == MC_INVALID)
        UNREACHABLE("Invalid math-constant-type!");

      printf("\033[36m%s\033[0m (\033[33m" DOUBLE_PRINT_FORMAT "\033[0m, %s)", mathConstantTypeIdentifiers[mc], mathConstantTypeValues[mc], mathConstantTypeNames[mc]);
    }
    break;
    case CN_VARIABLE:
    {
      node_variable_t *variable = vs_find_variable_ex(vs, &node->as.constant.as.variableName, NULL, FVM_CONST);

      if (variable)
      {
        printf("\033[35m" SS_FMT "\033[0m", SS_ARG(&node->as.constant.as.variableName));

        if (vs_call_stack_name_exists(callStack, &variable->as.varDef))
        {
          printf(" -> \033[31m[Recursion Detected]\033[0m");
          break;
        }

        printf(" ->%s", indented ? "\n" : " ");

        vs_call_stack_push(arena, callStack, &variable->as.varDef);
        print_node_ex(arena, vs, variable->expression, indented, depth + 1, callStack);
        vs_call_stack_pop(callStack);
      }
      else
      {
        variable = vs_find_variable_func_arg(vs, &node->as.constant.as.variableName, callStack);

        // Function argument.
        if (variable)
          printf("\033[34m" SS_FMT "\033[0m", SS_ARG(&node->as.constant.as.variableName));
      }

      // Normal variable
      if (!variable)
      {
        printf("\033[35m" SS_FMT "\033[0m", SS_ARG(&node->as.constant.as.variableName));
        printf(" -> \033[31m[Variable Not Found]\033[0m");
      }
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
    print_node_ex(arena, vs, node->as.binop.lhs, indented, depth + 1, callStack);
    printf(",%s", indented ? "\n" : " ");
    print_node_ex(arena, vs, node->as.binop.rhs, indented, depth + 1, callStack);
    if (indented)
      printf("\n");
    _PRINT_DEPTH_SPACES(indented, depth);
    printf(")");
  }
  break;
  case NT_FUNCTION:
  {
    node_func_t *funcNode = &node->as.func;

    bool isError = false;

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
      printf("\033[35m" SS_FMT "\033[0m", SS_ARG(&funcNode->as.variableName));

      node_variable_t *variable = vs_find_variable_ex(vs, &funcNode->as.variableName, NULL, FVM_FUNC);
      if (!variable)
      {
        printf("(...) -> ");
        printf("\033[31m[Function Not Found]\033[0m");
        isError = true;
        break;
      }

      if (vs_call_stack_name_exists(callStack, &funcNode->as.variableName))
      {
        printf("(...) -> ");
        printf("\033[31m[Recursion Detected]\033[0m");
        isError = true;
        break;
      }

      printf("(");
    }
    break;
    case FN_COUNT:
    default:
      UNREACHABLE("Invalid func-node-type!");
    }

    if (isError)
      break;

    node_func_args_t *args = &funcNode->args;

    if (args->count <= 0)
      UNREACHABLE("No arguments found in function node!");

    if (args->count == 1)
    {
      node_t *arg = args->items[0];

      bool isArgTypeConst = arg->type == NT_CONSTANT && arg->as.constant.type != CN_VARIABLE;

      if (indented && !isArgTypeConst)
        printf("\n");

      if (funcNode->type == FN_VARIABLE)
        vs_call_stack_push(arena, callStack, &funcNode->as.variableName);
      print_node_ex(arena, vs, arg, indented, !isArgTypeConst ? depth + 1 : 0, callStack);
      if (funcNode->type == FN_VARIABLE)
        vs_call_stack_pop(callStack);

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
        if (funcNode->type == FN_VARIABLE)
          vs_call_stack_push(arena, callStack, &funcNode->as.variableName);
        print_node_ex(arena, vs, args->items[i], indented, depth + 1, callStack);
        if (funcNode->type == FN_VARIABLE)
          vs_call_stack_pop(callStack);

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

    print_node_ex(arena, vs, child, indented, !isArgTypeConst ? depth + 1 : 0, callStack);

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

void print_node(arena_t *arena, node_variable_stack_t *vs, node_t *node, bool indented)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(vs);
  ASSERT_NULL(node);

  vs_call_stack_t callStack = {0};

  printf("Printing expression node:\n");
  print_node_ex(arena, vs, node, indented, 0, &callStack);

  assert(callStack.count == 0);

  printf("\n");
}

void print_node_simple(arena_t *arena, node_t *node, bool indented)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(node);

  node_variable_stack_t vs = {0};
  print_node(arena, &vs, node, indented);
}

void print_parser(arena_t *arena, parser_t *parser, bool indented, bool showVariableValues)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(parser);

  print_variables(arena, &parser->vs, showVariableValues, indented);
  printf("\n");

  if (!parser_contains_expressions(parser))
  {
    printf("No expressions found to print.\n");
    return;
  }

  printf("Printing expression nodes (%zu):\n", parser->exprs.count);

  vs_call_stack_t callStack = {0};

  for (size_t i = 0; i < parser->exprs.count; ++i)
  {
    printf("\033[30mEXPR-%zu\033[0m ->%s", i + 1, indented ? "\n" : " ");
    print_node_ex(arena, &parser->vs, node_exprs_at(&parser->exprs, i), indented, 1, &callStack);
    printf("\n");

    if (indented && i < parser->exprs.count - 1)
      printf("\n");

    assert(callStack.count == 0);
  }

  printf("\n");
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
  printf("\n");

  if (verbose)
  {
    printf("Internal tree:\n\n");
    print_node(arena, vs, test->expressionTree, indented);
    printf("\n");
  }

  tokenizer_t tokenizer = tokenizer_execute_cstr(arena, test->input);
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

  bool isError = false;
  node_t *generatedTree = try_parse_expression_without_equals(arena, &lexer, &isError);
  if (!generatedTree || isError)
  {
    if (freeArena)
      arena_free(arena);

    return false;
  }

  if (verbose)
  {
    printf("Generated tree:\n\n");
    print_node(arena, vs, generatedTree, indented);
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
