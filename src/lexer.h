#ifndef _LEXER_H_
#define _LEXER_H_

#include <arena.h>
#include <stringslice.h>
#include <chelpers.h>

#define T_ERROR_NAME "TOKENIZATION-ERROR"
#define T_ERROR_NO_INPUT_GIVEN() \
  fprintf(stderr, T_ERROR_NAME ": No input given!\n")
#define T_ERROR_GIVEN_TOKENIZER_INVALID() \
  fprintf(stderr, T_ERROR_NAME ": Can't print the tokenizer because an error happend!\n")

#define L_ERROR_NAME "LEXING-ERROR"
#define L_ERROR_INVALID_AMOUNT_OF_COMMAS_IN_NUMBER(cursor, tok) \
  fprintf(stderr, L_ERROR_NAME ":%zu: A number can only contain 1 comma ('" IN_TOK_FMT "')!\n", (cursor), IN_TOK_ARG(tok))
#define L_ERROR_INVALID_NUMBER(cursor, tok) \
  fprintf(stderr, L_ERROR_NAME ":%zu: Not a number ('" IN_TOK_FMT "')!\n", (cursor), IN_TOK_ARG(tok))
#define L_ERROR_INVALID_TOKEN(cursor, tok) \
  fprintf(stderr, L_ERROR_NAME ":%zu: '" IN_TOK_FMT "' is an invalid token!\n", (cursor), IN_TOK_ARG(tok))
#define L_ERROR_GIVEN_LEXER_INVALID() \
  fprintf(stderr, L_ERROR_NAME ": Can't print the lexer because an error happend!\n")

typedef enum
{
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
    [OP_POW] = '^',
};

const int operatorPrecedence[OP_COUNT] = {
    [OP_ADD] = 0,
    [OP_SUB] = 0,
    [OP_MUL] = 1,
    [OP_DIV] = 1,
    [OP_POW] = 2,
};

const bool operatorRightAssociative[OP_COUNT] = {
    [OP_ADD] = false,
    [OP_SUB] = false,
    [OP_MUL] = false,
    [OP_DIV] = false,
    [OP_POW] = true,
};

const char *operatorTypeNames[OP_COUNT] = {
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
      return (e_operator_type)i;
  }

  return OP_INVALID;
}

e_operator_type cstr_to_operator_type(const char *cstr)
{
  if (!cstr || strlen(cstr) != 1)
    return OP_INVALID;

  return char_to_operator_type(cstr[0]);
}

e_operator_type cstr_to_operator_type_ex(const char *cstr, size_t len)
{
  if (!cstr || len != 1)
    return OP_INVALID;

  return char_to_operator_type(cstr[0]);
}

#define c_is_operator(c) (char_to_operator_type(c) != OP_INVALID)
#define cstr_is_operator(cstr) (cstr_to_operator_type(cstr) != OP_INVALID)
#define cstr_is_operator_ex(cstr, len) (cstr_to_operator_type_ex(cstr, len) != OP_INVALID)

// Reference: https://en.wikipedia.org/wiki/List_of_mathematical_constants
typedef enum
{
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

const char *mathConstantTypeIdentifiers[MC_COUNT] = {
    [MC_PI] = "PI",              // Pi
    [MC_TAU] = "TAU",            // Tau
    [MC_PHI] = "PHI",            // Phi
    [MC_EULERS_NUMBER] = "EN",   // Euler's number
    [MC_EULERS_CONSTANT] = "EC", // Euler's constant
    [MC_OMEGA_CONSTANT] = "OC",  // Omega constant
    [MC_GAUSS_CONSTANT] = "GC",  // Gauss's constant
};

// A double can store 15 decimal digits.
const double mathConstantTypeValues[MC_COUNT] = {
    [MC_PI] = 3.141592653589793,  // Ratio of a circle's circumference to its diameter.
    [MC_TAU] = 6.283185307179586, // Ratio of a circle's circumference to its radius. Equivalent to 2 PI.
    [MC_PHI] = 1.618033988749894, // The golden ration "(1 + sqrt(5)) / 5"
    [MC_EULERS_NUMBER] = 2.718281828459045,
    [MC_EULERS_CONSTANT] = 0.577215664901532,
    [MC_OMEGA_CONSTANT] = 0.567143290409783,
    [MC_GAUSS_CONSTANT] = 0.834626841674073,
};

const char *mathConstantTypeNames[MC_COUNT] = {
    [MC_PI] = "Pi",
    [MC_TAU] = "Tau",
    [MC_PHI] = "Phi",
    [MC_EULERS_NUMBER] = "Euler's number",
    [MC_EULERS_CONSTANT] = "Euler's constant",
    [MC_OMEGA_CONSTANT] = "Omega constant",
    [MC_GAUSS_CONSTANT] = "Gauss's constant",
};

e_math_constant_type cstr_to_math_constant_type_ex(const char *cstr, size_t len)
{
  if (!cstr || len == 0)
    return MC_INVALID;

  for (size_t i = 0; i < MC_COUNT; ++i)
    if (strlen(mathConstantTypeIdentifiers[i]) == len && strncmp(cstr, mathConstantTypeIdentifiers[i], len) == 0)
      return (e_math_constant_type)i;

  return MC_INVALID;
}

e_math_constant_type cstr_to_math_constant_type(const char *cstr)
{
  if (!cstr)
    return MC_INVALID;

  for (size_t i = 0; i < MC_COUNT; ++i)
    if (strcmp(cstr, mathConstantTypeIdentifiers[i]) == 0)
      return (e_math_constant_type)i;

  return MC_INVALID;
}

#define cstr_is_math_constant(cstr) (cstr_to_math_constant_type(cstr) != MC_INVALID)
#define cstr_is_math_constant_ex(cstr, len) (cstr_to_math_constant_type_ex(cstr, len) != MC_INVALID)

// TODO: Implement parameter count for each function to support multiple arguments.
typedef enum
{
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
  FT_ATAN2,

  FT_LN,
  FT_LOG10,

  FT_COUNT,
  FT_INVALID
} e_function_type;

static_assert(FT_COUNT == 14, "Amount of function-types have changed");

const char *functionTypeIdentifiers[FT_COUNT] = {
    // Other
    [FT_SQRT] = "sqrt",
    [FT_EXP] = "exp",
    // Sin
    [FT_SIN] = "sin",
    [FT_ASIN] = "asin",
    [FT_SINH] = "sinh",
    // Cos
    [FT_COS] = "cos",
    [FT_ACOS] = "acos",
    [FT_COSH] = "cosh",
    // Tan
    [FT_TAN] = "tan",
    [FT_ATAN] = "atan",
    [FT_TANH] = "tanh",
    [FT_ATAN2] = "atan2",
    // Log
    [FT_LN] = "ln",
    [FT_LOG10] = "log10",
};

const size_t functionTypeParameterCount[FT_COUNT] = {
    // Other
    [FT_SQRT] = 1,
    [FT_EXP] = 1,
    // Sin
    [FT_SIN] = 1,
    [FT_ASIN] = 1,
    [FT_SINH] = 1,
    // Cos
    [FT_COS] = 1,
    [FT_ACOS] = 1,
    [FT_COSH] = 1,
    // Tan
    [FT_TAN] = 1,
    [FT_ATAN] = 1,
    [FT_TANH] = 1,
    [FT_ATAN2] = 2,
    // Log
    [FT_LN] = 1,
    [FT_LOG10] = 1,
};

const char *functionTypeNames[FT_COUNT] = {
    // Other
    [FT_SQRT] = "Square-Root",
    [FT_EXP] = "EN^x",
    // Sin
    [FT_SIN] = "Sinus",
    [FT_ASIN] = "Arcus-Sinus",
    [FT_SINH] = "Hyperbolic-Sinus",
    // Cos
    [FT_COS] = "Cosinus",
    [FT_ACOS] = "Arcus-Cosinus",
    [FT_COSH] = "Hyperbolic-Cosinus",
    // Tan
    [FT_TAN] = "Tangents",
    [FT_ATAN] = "Arcus-Tangents",
    [FT_TANH] = "Hyperbolic-Tangents",
    [FT_ATAN2] = "Arcus-Tangent-2",
    // Log
    [FT_LN] = "Natural-Logarithm",
    [FT_LOG10] = "Logarithm",
};

const char *functionTypeDescriptions[FT_COUNT] = {
    // Other
    [FT_SQRT] = "Returns the square root of x.",
    [FT_EXP] = "Returns the value of e raised to the x'th power.",
    // Sin
    [FT_SIN] = "Returns the sine of a radian angle x.",
    [FT_ASIN] = "Returns the arc sine of x in radians.",
    [FT_SINH] = "Returns the hyperbolic sine of x.",
    // Cos
    [FT_COS] = "Returns the cosine of a radian angle x.",
    [FT_ACOS] = "Returns the arc cosine of x in radians.",
    [FT_COSH] = "Returns the hyperbolic cosine of x.",
    // Tan
    [FT_TAN] = "Returns the tangent of a given angle x.",
    [FT_ATAN] = "Returns the arc tangent of x in radians.",
    [FT_TANH] = "Returns the hyperbolic tangent of x.",
    [FT_ATAN2] = "Returns the angle theta from the conversion of rectangular coordinates "
                 "(x, y) to polar coordinates (r, theta).",
    // Log
    [FT_LN] = "Returns the natural logarithm (base-e logarithm) of x.",
    [FT_LOG10] = "Returns the common logarithm (base-10 logarithm) of x.",
};

e_function_type cstr_to_function_type_ex(const char *cstr, size_t len)
{
  if (!cstr || len == 0)
    return FT_INVALID;

  for (size_t i = 0; i < FT_COUNT; ++i)
    if (strlen(functionTypeIdentifiers[i]) == len && strncmp(cstr, functionTypeIdentifiers[i], len) == 0)
      return (e_function_type)i;

  return FT_INVALID;
}

e_function_type cstr_to_function_type(const char *cstr)
{
  if (!cstr)
    return FT_INVALID;

  for (size_t i = 0; i < FT_COUNT; ++i)
    if (strcmp(cstr, functionTypeIdentifiers[i]) == 0)
      return (e_function_type)i;

  return FT_INVALID;
}

#define cstr_is_function(cstr) (cstr_to_function_type(cstr) != FT_INVALID)
#define cstr_is_function_ex(cstr, len) (cstr_to_function_type_ex(cstr, len) != FT_INVALID)

typedef enum
{
  PT_OPAREN,
  PT_CPAREN,

  PT_COUNT,
  PT_INVALID
} e_paren_type;

static_assert(PT_COUNT == 2, "Amount of paren-types have changed");

const char parenTypeIdentifiers[PT_COUNT] = {
    [PT_OPAREN] = '(',
    [PT_CPAREN] = ')',
};

const char *parenTypeNames[PT_COUNT] = {
    [PT_OPAREN] = "Open",
    [PT_CPAREN] = "Close",
};

e_paren_type char_to_paren_type(const char c)
{
  for (size_t i = 0; i < PT_COUNT; ++i)
  {
    if (parenTypeIdentifiers[i] == c)
      return (e_paren_type)i;
  }

  return PT_INVALID;
}

e_paren_type cstr_to_paren_type_ex(const char *cstr, size_t len)
{
  if (!cstr || len != 1)
    return PT_INVALID;

  return char_to_paren_type(cstr[0]);
}

e_paren_type cstr_to_paren_type(const char *cstr)
{
  if (!cstr || strlen(cstr) != 1)
    return PT_INVALID;

  return char_to_paren_type(cstr[0]);
}

#define c_is_paren(c) (char_to_paren_type(c) != PT_INVALID)
#define cstr_is_paren_ex(cstr, len) (cstr_to_paren_type_ex(cstr, len) != PT_INVALID)
#define cstr_is_paren(cstr) (cstr_to_paren_type(cstr) != PT_INVALID)

// TODO: Rethink! Maybe add later here also !=, <=, >=, <, > or add it to symbols.
typedef enum
{
  CLT_COMMA,
  CLT_EQUALS,

  CLT_COUNT,
  CLT_INVALID
} e_common_literal_type;

static_assert(CLT_COUNT == 2, "Amount of common-literal-types have changed");

const char commonLiteralTypeIdentifiers[CLT_COUNT] = {
    [CLT_COMMA] = ',',
    [CLT_EQUALS] = '=',
};

const char *commonLiteralTypeNames[CLT_COUNT] = {
    [CLT_COMMA] = "Comma",
    [CLT_EQUALS] = "Equals",
};

e_common_literal_type char_to_common_literal_type(const char c)
{
  for (size_t i = 0; i < CLT_COUNT; ++i)
  {
    if (commonLiteralTypeIdentifiers[i] == c)
      return (e_common_literal_type)i;
  }

  return CLT_INVALID;
}

e_common_literal_type cstr_to_common_literal_type_ex(const char *cstr, size_t len)
{
  if (!cstr || len != 1)
    return CLT_INVALID;

  return char_to_common_literal_type(cstr[0]);
}

e_common_literal_type cstr_to_common_literal_type(const char *cstr)
{
  if (!cstr || strlen(cstr) != 1)
    return CLT_INVALID;

  return char_to_common_literal_type(cstr[0]);
}

#define c_is_common_literal(c) (char_to_common_literal_type(c) != CLT_INVALID)
#define cstr_is_common_literal_ex(cstr, len) (cstr_to_common_literal_type_ex(cstr, len) != CLT_INVALID)
#define cstr_is_common_literal(cstr) (cstr_to_common_literal_type(cstr) != CLT_INVALID)

// Literals are all single character tokens.
#define c_is_literal(c) (c_is_operator(c) || c_is_paren(c) || c_is_common_literal(c))

// Input-Token printing macros
#define IN_TOK_FMT "%.*s"
#define IN_TOK_ARG(tok) (int)(tok)->length, (tok)->value

typedef struct
{
  const char *value;
  size_t length;
  size_t cursor;
} input_token_t;

typedef struct
{
  input_token_t *items;
  size_t capacity;
  size_t count;
  bool isError;
} tokenizer_t;

#define tokenizer_append(a, tokenizer, val, len, curr) \
  arena_da_append((a), (tokenizer), ((input_token_t){.value = (val), .length = (len), .cursor = (curr)}))

// Tokenizes a collection of characters into a symbol which are not spaces and literal characters.
static bool next_symbol(arena_t *arena, string_slice_t *ss, tokenizer_t *tokenizer)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(ss);
  ASSERT_NULL(tokenizer);

  if (!ss_in_range(ss))
    return true;

  const char *currentPtr = ss_get_current_ptr(ss);
  char current;
  size_t length = 0;

  do
  {
    current = ss_get_current(ss);

    // If the current character is a space or a literal character, we stop the symbol tokenization.
    if (isspace(current) || c_is_literal(current))
      break;

    ++length;
    ss_seek(ss);
  } while (ss_in_range(ss));

  if (length > 0)
    tokenizer_append(arena, tokenizer, currentPtr, length, ss_current_pos(ss) - length);

  return length > 0;
}

// Tokenizes a single character literal if valid.
static bool next_literal(arena_t *arena, string_slice_t *ss, tokenizer_t *tokenizer)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(ss);
  ASSERT_NULL(tokenizer);

  if (!ss_in_range(ss))
    return true;

  const char *currentPtr = ss_get_current_ptr(ss);
  bool isLiteral = c_is_literal(*currentPtr);

  if (isLiteral)
    tokenizer_append(arena, tokenizer, currentPtr, 1, ss_current_pos(ss));

  ss_seek(ss);

  return isLiteral;
}

tokenizer_t tokenizer_execute(arena_t *arena, const char *input)
{
  ASSERT_NULL(arena);

  tokenizer_t tokenizer = {0};

  if (!input || !strlen(input))
  {
    T_ERROR_NO_INPUT_GIVEN();
    tokenizer.isError = true;
    return tokenizer;
  }

  string_slice_t ss = {0};
  ss_init(&ss, input);

  while (ss_in_range(&ss))
  {
    ss_seek_spaces(&ss);

    if (next_symbol(arena, &ss, &tokenizer) || next_literal(arena, &ss, &tokenizer))
      continue;

    // Error
    UNREACHABLE_MSG("Unhandled character!");
    tokenizer.isError = true;
    break;
  }

  return tokenizer;
}

void tokenizer_print(const tokenizer_t *tokenizer)
{
  ASSERT_NULL(tokenizer);

  if (tokenizer->isError || tokenizer->count == 0)
  {
    T_ERROR_GIVEN_TOKENIZER_INVALID();
    return;
  }

  printf("Printing tokenenized input (%zu tokens):\n", tokenizer->count);

  for (size_t i = 0; i < tokenizer->count; i++)
    printf("Token('" IN_TOK_FMT "')\n", IN_TOK_ARG(&tokenizer->items[i]));
}

typedef enum
{
  // Only numbers like '109', '53.23531'
  TT_NUMBER,
  // All operators like '+', '-', ...
  TT_OPERATOR,
  // All paren types
  TT_PAREN,
  // Here are all not connected literals like ',' or '='
  TT_LITERAL,
  // All for example variable names.
  TT_SYMBOL,

  // TODO: Rethink both!
  // TT_NEW_LINE,
  // TT_EOF,

  // Count of all token types.
  TT_COUNT
} e_token_type;

static_assert(TT_COUNT == 5, "Amount of token-types have changed");

static const char *tokenTypeNames[TT_COUNT] = {
    [TT_NUMBER] = "Number",
    [TT_OPERATOR] = "Operator",
    [TT_PAREN] = "Parenthesis",
    [TT_LITERAL] = "Literal",
    [TT_SYMBOL] = "Symbol",
};

typedef union
{
  // TODO: Change to big number type!
  double number;
  e_operator_type operator;
  e_paren_type paren;
  e_common_literal_type literal;
  string_slice_t symbol;
} u_token_as;

typedef struct
{
  e_token_type type;
  size_t cursor;
  u_token_as as;
} token_t;

typedef struct
{
  token_t *items;
  size_t capacity;
  size_t count;
  bool isError;

  // TODO: Rethink! Could be used to internally traverse from first to last token without having to check an external variable.
  size_t cursor;
} lexer_t;

#define add_number_token(a, lexer, num, curr) \
  arena_da_append((a), (lexer), ((token_t){.type = TT_NUMBER, .as.number = (num), .cursor = (curr)}))
#define add_operator_token(a, lexer, op, curr) \
  arena_da_append((a), (lexer), ((token_t){.type = TT_OPERATOR, .as.operator=(op), .cursor = (curr)}))
#define add_paren_token(a, lexer, pt, curr) \
  arena_da_append((a), (lexer), ((token_t){.type = TT_PAREN, .as.paren = (pt), .cursor = (curr)}))
#define add_literal_token(a, lexer, clt, curr) \
  arena_da_append((a), (lexer), ((token_t){.type = TT_LITERAL, .as.literal = (clt), .cursor = (curr)}))
#define add_symbol_token(a, lexer, sym, curr) \
  arena_da_append((a), (lexer), ((token_t){.type = TT_SYMBOL, .as.symbol = (sym), .cursor = (curr)}))

#define lex_at(lexer, idx) (&(lexer)->items[(idx)])
#define lex_in_range(lexer, idx) ((idx) < (lexer)->count - 1)
#define lex_next_in_range(lexer, idx) ((idx) + 1 < (lexer)->count - 1)

#define tok_is(tok, t) ((tok)->type == (t))
#define tok_not(tok, t) ((tok)->type != (t))

#define tok_is_paren(tok, pt) \
  (tok_is(tok, TT_PAREN) && (tok)->as.paren == (pt))
#define tok_not_specific_paren(tok, pt) \
  (tok_not((tok), TT_PAREN) || (tok)->as.paren != (pt))
#define tok_is_number_operator(tok) \
  (tok_is(tok, TT_OPERATOR) && ((tok)->as.operator== OP_ADD ||(tok)->as.operator== OP_SUB))
#define tok_is_specific_literal(tok, lt) \
  (tok_is(tok, TT_LITERAL) && (tok)->as.literal == (lt))
#define tok_not_specific_literal(tok, lt) \
  (tok_not(tok, TT_LITERAL) || (tok)->as.literal != (lt))

lexer_t lexer_execute(arena_t *arena, tokenizer_t *tokenizer)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(tokenizer);

  lexer_t lexer = {0};

  for (size_t i = 0; i < tokenizer->count; ++i)
  {
    const input_token_t *currentToken = &tokenizer->items[i];

    // Operator needs to be checked at first because else the double conversion fails on number parsing.
    if (cstr_is_operator_ex(currentToken->value, currentToken->length))
    {
      e_operator_type op = cstr_to_operator_type_ex(currentToken->value, currentToken->length);

      if (op == OP_INVALID)
        UNREACHABLE("Invalid operator-type!");

      add_operator_token(arena, &lexer, op, currentToken->cursor);
      continue;
    }

    // Number checking
    num_check_t numCheck = cstr_is_number_ex(currentToken->value, currentToken->length);
    switch (numCheck.ret)
    {
    case NCR_NULL_OR_EMPTY:
      UNREACHABLE("Number-Token was NULL!");
      break;
    case NCR_VALID:
    {
      char *endptr;
      double number = strtof(currentToken->value, &endptr);

      if (!endptr || endptr == currentToken->value || (size_t)(endptr - currentToken->value) != currentToken->length)
        UNREACHABLE("Error while converting a number!");

      add_number_token(arena, &lexer, number, currentToken->cursor);
      continue;
    }
    break;
    case NCR_TOO_MANY_COMMAS:
    {
      L_ERROR_INVALID_AMOUNT_OF_COMMAS_IN_NUMBER(currentToken->cursor + numCheck.cursor, currentToken);
      lexer.isError = true;
      continue;
    }
    break;
    case NCR_NOT_A_NUMBER:
      // Literal, symbol, paren, ...
      break;
    case NCR_COUNT:
    default:
      UNREACHABLE("Invalid number check return value!");
      break;
    }

    if (cstr_is_paren_ex(currentToken->value, currentToken->length))
    {
      e_paren_type pt = cstr_to_paren_type_ex(currentToken->value, currentToken->length);

      if (pt == PT_INVALID)
        UNREACHABLE("Invalid paren-type!");

      add_paren_token(arena, &lexer, pt, currentToken->cursor);
      continue;
    }

    if (cstr_is_common_literal_ex(currentToken->value, currentToken->length))
    {
      e_common_literal_type clt = cstr_to_common_literal_type_ex(currentToken->value, currentToken->length);

      if (clt == CLT_INVALID)
        UNREACHABLE("Invalid common-literal-type!");

      add_literal_token(arena, &lexer, clt, currentToken->cursor);
      continue;
    }

    // variable names, math constants, function names, ...
    string_slice_t symbol = {0};
    ss_init_ex(&symbol, currentToken->value, currentToken->length);
    add_symbol_token(arena, &lexer, symbol, currentToken->cursor);
  }

  return lexer;
}

void lexer_print(const lexer_t *lexer)
{
  ASSERT_NULL(lexer);

  if (lexer->isError || lexer->count == 0)
  {
    L_ERROR_GIVEN_LEXER_INVALID();
    return;
  }

  printf("Printing lexed tokens (%zu tokens):\n", lexer->count);

  for (size_t i = 0; i < lexer->count; ++i)
  {
    token_t *token = lex_at(lexer, i);

    printf("%s", tokenTypeNames[token->type]);
    switch (token->type)
    {
    case TT_NUMBER:
      printf("(" DOUBLE_PRINT_FORMAT ")", token->as.number);
      break;
    case TT_OPERATOR:
        printf("(%s): precedence: %d, right-associative: %s", operatorTypeNames[token->as.operator], operatorPrecedence[token->as.operator], bool_cstr(operatorRightAssociative[token->as.operator]));
        break;
    case TT_PAREN:
      printf("(%s)", parenTypeNames[token->as.paren]);
      break;
    case TT_LITERAL:
      printf("(%s)", commonLiteralTypeNames[token->as.literal]);
      break;
    case TT_SYMBOL:
      printf("('" SS_FMT "')", SS_ARG(&token->as.symbol));
      if (cstr_is_math_constant_ex(token->as.symbol.src, token->as.symbol.len))
      {
        e_math_constant_type mc = cstr_to_math_constant_type_ex(token->as.symbol.src, token->as.symbol.len);
        printf(": Math-Constant(" DOUBLE_PRINT_FORMAT ", %s)", mathConstantTypeValues[mc], mathConstantTypeNames[mc]);
      }
      else if (cstr_is_function_ex(token->as.symbol.src, token->as.symbol.len))
      {
        e_function_type ft = cstr_to_function_type_ex(token->as.symbol.src, token->as.symbol.len);
        printf(": Function(%s)", functionTypeNames[ft]);
      }
      break;
    case TT_COUNT:
    default:
      UNREACHABLE("Invalid token-type!");
      break;
    }
    printf("\n");
  }
}

#endif // _LEXER_H_
