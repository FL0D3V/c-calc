#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)
#define ASSERT_NULL(value) assert((value) && #value)


#define COMMA_CHARACTER '.'

#define c_is_comma(c)     ((c) == COMMA_CHARACTER)
#define c_is_literal(c)   (isdigit(c) || c_is_comma(c))

typedef struct {
  int ret;
  int cursor;
} lit_check_ret_t;

#define literal_check_ret_null()          ((lit_check_ret_t) { .ret =  1, .cursor = 0     })
#define literal_check_ret_success(cur)    ((lit_check_ret_t) { .ret =  0, .cursor = (cur) })
#define literal_check_ret_comma_err(cur)  ((lit_check_ret_t) { .ret = -1, .cursor = (cur) })
#define literal_check_ret_invalid(cur)    ((lit_check_ret_t) { .ret = -2, .cursor = (cur) })

// Returns an 'lit_check_ret_t'.
// The 'ret' value variations:
//  1: Input was NULL or empty. Cursor will be set to 0.
//  0: Input IS a valid literal and the cursor will be set to the length of the input.
// -1: Input has to many commas. Cursor will be set to the error position in the string.
// -2: Input was NOT a literal. Cursor will be set to the error position in the string.
lit_check_ret_t cstr_is_literal(const char* cstr)
{
  if (!cstr)
    return literal_check_ret_null();

  size_t len = strlen(cstr);
  if (len <= 0)
    return literal_check_ret_null();

  bool commaFound = false;

  for (size_t i = 0; i < len; ++i)
  {
    const char current = cstr[i];
    
    if (!c_is_literal(current))
      return literal_check_ret_invalid(i);

    if (c_is_comma(current))
    {
      if (commaFound)
        return literal_check_ret_comma_err(i);
      else
        commaFound = true;
    }
  }

  return literal_check_ret_success(len);
}


typedef enum {
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,

  OP_COUNT,
  OP_INVALID
} e_operator_type;

static_assert(OP_COUNT == 4, "Amount of operator-types have changed");
const char* operatorTypeNames[OP_COUNT] = {
	[OP_ADD] = "Add",
	[OP_SUB] = "Subtract",
	[OP_MUL] = "Multiply",
	[OP_DIV] = "Divide",
};

e_operator_type char_to_operator_type(const char c)
{
  switch (c)
  {
    case '+': return OP_ADD;
    case '-': return OP_SUB;
    case '*': return OP_MUL;
    case '/': return OP_DIV;
    default:  return OP_INVALID;
  }
}

e_operator_type cstr_to_operator_type(const char* cstr)
{
  if (!cstr || strlen(cstr) != 1)
    return OP_INVALID;

  return char_to_operator_type(cstr[0]);
}

#define c_is_operator(c) (char_to_operator_type(c) != OP_INVALID)
#define cstr_is_operator(cstr) (cstr_to_operator_type(cstr) != OP_INVALID)


typedef enum {
  BT_OPAREN,
  BT_CPAREN,

  BT_COUNT,
  BT_INVALID
} e_bracket_type;

static_assert(BT_COUNT == 2, "Amount of bracket-types have changed");
const char* bracketTypeNames[BT_COUNT] = {
  [BT_OPAREN] = "Open-Paren",
  [BT_CPAREN] = "Closing-Paren"
};

e_bracket_type char_to_bracket_type(const char c)
{
  switch (c)
  {
    case '(': return BT_OPAREN;
    case ')': return BT_CPAREN;
    default:  return BT_INVALID;
  }
}

e_bracket_type cstr_to_bracket_type(const char* cstr)
{
  if (!cstr || strlen(cstr) != 1)
    return BT_INVALID;

  return char_to_bracket_type(cstr[0]);
}

#define c_is_bracket(c) (char_to_bracket_type(c) != BT_INVALID)
#define cstr_is_bracket(cstr) (cstr_to_bracket_type(cstr) != BT_INVALID)



typedef enum {
  FT_SQRT,
  FT_POW,

  FT_COUNT,
  FT_INVALID
} e_function_type;

static_assert(FT_COUNT == 2, "Amount of function-types have changed");
const char* functionTypeNames[FT_COUNT] = {
  [FT_SQRT] = "Sqrt",
  [FT_POW] = "Pow"
};

e_function_type cstr_to_function_type(const char* cstr)
{
  if (!cstr)
    return FT_INVALID;

  for (size_t i = 0; i < FT_COUNT; ++i)
    if (strcmp(cstr, functionTypeNames[i]) == 0)
      return (e_function_type) i;

  return FT_INVALID;
}

#define cstr_is_function(cstr) (cstr_to_function_type(cstr) != FT_INVALID)

#endif // _GLOBAL_H_
