#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>


// TODO: Does currently not print correctly after around 5 decimal digits!
#define DOUBLE_PRINT_FORMAT "%.05lf"


#define _DECIMAL_SEPERATOR_CHARACTER '.'
#define _MINUS_CHARACTER '-'

#define c_is_decimal_seperator(c) ((c) == _DECIMAL_SEPERATOR_CHARACTER)
#define c_is_number(c)            (isdigit(c) || c_is_decimal_seperator(c))

typedef struct {
  int ret;
  int cursor;
} num_check_t;

#define _NUM_CHECK_NULL()         ((num_check_t) { .ret =  1, .cursor = 0     })
#define _NUM_CHECK_SUCCESS(cur)   ((num_check_t) { .ret =  0, .cursor = (cur) })
#define _NUM_CHECK_COMMA_ERR(cur) ((num_check_t) { .ret = -1, .cursor = (cur) })
#define _NUM_CHECK_INVALID(cur)   ((num_check_t) { .ret = -2, .cursor = (cur) })

// Returns an 'num_check_t'.
// The 'ret' value variations:
//  1: Input was NULL or empty. Cursor will be set to 0.
//  0: Input IS a valid number and the cursor will be set to the length of the input.
// -1: Input has too many commas. Cursor will be set to the error position in the string.
// -2: Input was NOT a number. Cursor will be set to the error position in the string.
num_check_t cstr_is_number_ex(const char* cstr, size_t len)
{
  if (!cstr || len == 0)
    return _NUM_CHECK_NULL();

  bool decimalSeperatorFound = false;

  // TODO: Rethink!
  char* current = (char*) cstr;
  
  for (size_t i = 0; i < len; ++i)
  {
    if (i == 0 && *current == _MINUS_CHARACTER)
      continue;
    
    if (!c_is_number(*current))
      return _NUM_CHECK_INVALID(i);

    if (c_is_decimal_seperator(*current))
    {
      if (decimalSeperatorFound)
        return _NUM_CHECK_COMMA_ERR(i);
      else
        decimalSeperatorFound = true;
    }

    current++;
  }

  return _NUM_CHECK_SUCCESS(len);
}

num_check_t cstr_is_number(const char* cstr)
{
  // Needs to check for null before trying to get the length.
  if (!cstr)
    return _NUM_CHECK_NULL();
  
  return cstr_is_number_ex(cstr, strlen(cstr));
}



// Reference: https://en.wikipedia.org/wiki/List_of_mathematical_constants
typedef enum {
  MC_PI,
  MC_TAU,
  MC_PHI,
  MC_EULERS_NUMBER,
  MC_EULERS_CONSTANT,
  MC_OMEGA_CONSTANT,
  MC_GAUSS_CONSTANT,

  MC_COUNT,
  MC_INVALID
} e_math_constant_type;

static_assert(MC_COUNT == 7, "Amount of math-constant-types have changed");

const char* mathConstantTypeIdentifiers[MC_COUNT] = {
  [MC_PI]              = "PI",  // Pi
  [MC_TAU]             = "TAU", // Tau
  [MC_PHI]             = "PHI", // Phi
  [MC_EULERS_NUMBER]   = "EN",  // Euler's number
  [MC_EULERS_CONSTANT] = "EC",  // Euler's constant
  [MC_OMEGA_CONSTANT]  = "OC",  // Omega constant
  [MC_GAUSS_CONSTANT]  = "GC",  // Gauss's constant
};

// A double can store 15 decimal digits.
const double mathConstantTypeValues[MC_COUNT] = {
  [MC_PI]              = 3.141592653589793, // Ratio of a circle's circumference to its diameter.
  [MC_TAU]             = 6.283185307179586, // Ratio of a circle's circumference to its radius. Equivalent to 2 PI.
  [MC_PHI]             = 1.618033988749894, // The golden ration "(1 + sqrt(5)) / 5"
  [MC_EULERS_NUMBER]   = 2.718281828459045,
  [MC_EULERS_CONSTANT] = 0.577215664901532,
  [MC_OMEGA_CONSTANT]  = 0.567143290409783,
  [MC_GAUSS_CONSTANT]  = 0.834626841674073,
};

const char* mathConstantTypeNames[MC_COUNT] = {
  [MC_PI]              = "Pi",
  [MC_TAU]             = "Tau",
  [MC_PHI]             = "Phi",
  [MC_EULERS_NUMBER]   = "Euler's number",
  [MC_EULERS_CONSTANT] = "Euler's constant",
  [MC_OMEGA_CONSTANT]  = "Omega constant",
  [MC_GAUSS_CONSTANT]  = "Gauss's constant",
};

e_math_constant_type cstr_to_math_constant_type(const char* cstr)
{
  if (!cstr)
    return MC_INVALID;

  for (size_t i = 0; i < MC_COUNT; ++i)
    if (strcmp(cstr, mathConstantTypeIdentifiers[i]) == 0)
      return (e_math_constant_type) i;

  return MC_INVALID;
}

e_math_constant_type cstr_to_math_constant_type_ex(const char* cstr, size_t len)
{
  if (!cstr || len == 0)
    return MC_INVALID;

  for (size_t i = 0; i < MC_COUNT; ++i)
    if (strncmp(cstr, mathConstantTypeIdentifiers[i], len) == 0)
      return (e_math_constant_type) i;

  return MC_INVALID;
}

#define cstr_is_math_constant(cstr)         (cstr_to_math_constant_type(cstr) != MC_INVALID)
#define cstr_is_math_constant_ex(cstr, len) (cstr_to_math_constant_type_ex(cstr, len) != MC_INVALID)


typedef enum {
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_POW,

  OP_COUNT,
  OP_INVALID
} e_operator_type;

static_assert(OP_COUNT == 5, "Amount of operator-types have changed");

const char operatorTypeIdentifiers[OP_COUNT] = {
  [OP_ADD] = '+',
  [OP_SUB] = '-',
  [OP_MUL] = '*',
  [OP_DIV] = '/',
  [OP_POW] = '^'
};

const char* operatorTypeNames[OP_COUNT] = {
	[OP_ADD] = "Add",
	[OP_SUB] = "Subtract",
	[OP_MUL] = "Multiply",
	[OP_DIV] = "Divide",
	[OP_POW] = "Pow",
};


e_operator_type char_to_operator_type(const char c)
{
  for (size_t i = 0; i < OP_COUNT; ++i)
  {
    if (operatorTypeIdentifiers[i] == c)
      return (e_operator_type) i;
  }

  return OP_INVALID;
}

e_operator_type cstr_to_operator_type(const char* cstr)
{
  if (!cstr || strlen(cstr) != 1)
    return OP_INVALID;

  return char_to_operator_type(cstr[0]);
}

e_operator_type cstr_to_operator_type_ex(const char* cstr, size_t len)
{
  if (!cstr || len != 1)
    return OP_INVALID;

  return char_to_operator_type(cstr[0]);
}


#define c_is_operator(c)               (char_to_operator_type(c) != OP_INVALID)
#define cstr_is_operator(cstr)         (cstr_to_operator_type(cstr) != OP_INVALID)
#define cstr_is_operator_ex(cstr, len) (cstr_to_operator_type_ex(cstr, len) != OP_INVALID)


// TODO: Implement parameter count for each function to support multiple arguments.
typedef enum {
  FT_SQRT,
  FT_EXP,
  
  FT_SIN,
  FT_ASIN,
  FT_SINH,

  FT_COS,
  FT_ACOS,
  FT_COSH,

  FT_TAN,
  FT_ATAN,
  FT_TANH,
  
  FT_LN,
  FT_LOG10,

  FT_COUNT,
  FT_INVALID
} e_function_type;

static_assert(FT_COUNT == 13, "Amount of function-types have changed");

const char* functionTypeIdentifiers[FT_COUNT] = {
  // Other
  [FT_SQRT]   = "sqrt",
  [FT_EXP]    = "exp",
  // Sin
  [FT_SIN]    = "sin",
  [FT_ASIN]   = "asin",
  [FT_SINH]   = "sinh",
  // Cos
  [FT_COS]    = "cos",
  [FT_ACOS]   = "acos",
  [FT_COSH]   = "cosh",
  // Tan
  [FT_TAN]    = "tan",
  [FT_ATAN]   = "atan",
  [FT_TANH]   = "tanh",
  // Log
  [FT_LN]     = "ln",
  [FT_LOG10]  = "log10"
};

const char* functionTypeNames[FT_COUNT] = {
  // Other
  [FT_SQRT]   = "Square-Root",
  [FT_EXP]    = "EN^x",
  // Sin
  [FT_SIN]    = "Sinus",
  [FT_ASIN]   = "Arcus-Sinus",
  [FT_SINH]   = "Hyperbolic-Sinus",
  // Cos
  [FT_COS]    = "Cosinus",
  [FT_ACOS]   = "Arcus-Cosinus",
  [FT_COSH]   = "Hyperbolic-Cosinus",
  // Tan
  [FT_TAN]    = "Tangents",
  [FT_ATAN]   = "Arcus-Tangents",
  [FT_TANH]   = "Hyperbolic-Tangents",
  // Log
  [FT_LN]     = "Natural-Logarithm",
  [FT_LOG10]  = "Logarithm"
};

const char* functionTypeDescriptions[FT_COUNT] = {
  // Other
  [FT_SQRT]   = "Returns the square root of x.",
  [FT_EXP]    = "Returns the value of e raised to the x'th power.",
  // Sin
  [FT_SIN]    = "Returns the sine of a radian angle x.",
  [FT_ASIN]   = "Returns the arc sine of x in radians.",
  [FT_SINH]   = "Returns the hyperbolic sine of x.",
  // Cos
  [FT_COS]    = "Returns the cosine of a radian angle x.",
  [FT_ACOS]   = "Returns the arc cosine of x in radians.",
  [FT_COSH]   = "Returns the hyperbolic cosine of x.",
  // Tan
  [FT_TAN]    = "Returns the tangent of a given angle x.",
  [FT_ATAN]   = "Returns the arc tangent of x in radians.",
  [FT_TANH]   = "Returns the hyperbolic tangent of x.",
  // Log
  [FT_LN]     = "Returns the natural logarithm (base-e logarithm) of x.",
  [FT_LOG10]  = "Returns the common logarithm (base-10 logarithm) of x."
};

e_function_type cstr_to_function_type_ex(const char* cstr, size_t len)
{
  if (!cstr || len == 0)
    return FT_INVALID;

  for (size_t i = 0; i < FT_COUNT; ++i)
    if (strncmp(cstr, functionTypeIdentifiers[i], len) == 0)
      return (e_function_type) i;

  return FT_INVALID;
}

e_function_type cstr_to_function_type(const char* cstr)
{
  if (!cstr)
    return FT_INVALID;

  for (size_t i = 0; i < FT_COUNT; ++i)
    if (strcmp(cstr, functionTypeIdentifiers[i]) == 0)
      return (e_function_type) i;

  return FT_INVALID;
}

#define cstr_is_function(cstr)         (cstr_to_function_type(cstr) != FT_INVALID)
#define cstr_is_function_ex(cstr, len) (cstr_to_function_type_ex(cstr, len) != FT_INVALID)



typedef enum {
  PT_OPAREN,
  PT_CPAREN,

  PT_COUNT,
  PT_INVALID
} e_paren_type;

static_assert(PT_COUNT == 2, "Amount of paren-types have changed");

const char parenTypeIdentifiers[PT_COUNT] = {
  [PT_OPAREN] = '(',
  [PT_CPAREN] = ')'
};

const char* parenTypeNames[PT_COUNT] = {
  [PT_OPAREN] = "Open",
  [PT_CPAREN] = "Close"
};

e_paren_type char_to_paren_type(const char c)
{
  for (size_t i = 0; i < PT_COUNT; ++i)
  {
    if (parenTypeIdentifiers[i] == c)
      return (e_paren_type) i;
  }

  return PT_INVALID;
}

e_paren_type cstr_to_paren_type(const char* cstr)
{
  if (!cstr || strlen(cstr) != 1)
    return PT_INVALID;

  return char_to_paren_type(cstr[0]);
}

e_paren_type cstr_to_paren_type_ex(const char* cstr, size_t len)
{
  if (!cstr || len != 1)
    return PT_INVALID;

  return char_to_paren_type(cstr[0]);
}


#define c_is_paren(c)               (char_to_paren_type(c) != PT_INVALID)
#define cstr_is_paren(cstr)         (cstr_to_paren_type(cstr) != PT_INVALID)
#define cstr_is_paren_ex(cstr, len) (cstr_to_paren_type_ex(cstr, len) != PT_INVALID)



// TODO: Rethink! Maybe add later here also !=, <=, >=, <, > or add it to symbols.
typedef enum {
  CLT_COMMA,
  CLT_EQUALS,

  CLT_COUNT,
  CLT_INVALID
} e_common_literal_type;

static_assert(CLT_COUNT == 2, "Amount of common-literal-types have changed");

const char commonLiteralTypeIdentifiers[CLT_COUNT] = {
  [CLT_COMMA] = ',',
  [CLT_EQUALS] = '='
};

const char* commonLiteralTypeNames[CLT_COUNT] = {
  [CLT_COMMA] = "Comma",
  [CLT_EQUALS] = "Equals"
};


e_common_literal_type char_to_common_literal_type(const char c)
{
  for (size_t i = 0; i < CLT_COUNT; ++i)
  {
    if (commonLiteralTypeIdentifiers[i] == c)
      return (e_common_literal_type) i;
  }

  return CLT_INVALID;
}

e_common_literal_type cstr_to_common_literal_type(const char* cstr)
{
  if (!cstr || strlen(cstr) != 1)
    return CLT_INVALID;

  return char_to_common_literal_type(cstr[0]);
}

e_common_literal_type cstr_to_common_literal_type_ex(const char* cstr, size_t len)
{
  if (!cstr || len != 1)
    return CLT_INVALID;

  return char_to_common_literal_type(cstr[0]);
}

#define c_is_common_literal(c)                (char_to_common_literal_type(c) != CLT_INVALID)
#define cstr_is_common_literal(cstr)          (cstr_to_common_literal_type(cstr) != CLT_INVALID)
#define cstr_is_common_literal_ex(cstr, len)  (cstr_to_common_literal_type_ex(cstr, len) != CLT_INVALID)

// Literals are all single character tokens.
#define c_is_literal(c) (c_is_operator(c) || c_is_paren(c) || c_is_common_literal(c))

#endif // _GLOBAL_H_
