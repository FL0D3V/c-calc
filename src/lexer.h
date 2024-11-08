// Used for handling the tokenization and lexing of an input math equation.

#ifndef _LEXER_H_
#define _LEXER_H_

#include "global.h"
#include "darray.h"
#include "stringslice.h"


#define l_return_defer()              do { isError = true; goto defer; } while (0)
#define l_unreachable_defer(message)  do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); l_return_defer(); } while(0)


#define L_ERROR_INVALID_LITERAL(cursor, cstr) fprintf(stderr, "lexer_error:%zu: A literal can only contain 1 comma ('%s')!\n", (cursor), (cstr))
#define L_ERROR_INVALID_TOKEN(cursor, cstr)   fprintf(stderr, "lexer_error:%zu: '%s' is an invalid token!\n", (cursor), (cstr))
#define L_ERROR_GIVEN_LEXER_INVALID()         fprintf(stderr, "lexer_error: Can't print the lexer because an error happend!\n")


typedef enum {
  TT_LITERAL,
  TT_OPERATOR,
  TT_BRACKET,
  TT_MATH_CONSTANT,
  TT_FUNCTION,

  TT_COUNT
} e_token_type;

static_assert(TT_COUNT == 5, "Amount of token-types have changed");
const char* tokenTypeNames[TT_COUNT] = {
	[TT_LITERAL] = "Literal",
	[TT_OPERATOR] = "Operator",
  [TT_BRACKET] = "Bracket",
  [TT_MATH_CONSTANT] = "MathConstant",
  [TT_FUNCTION] = "Function",
};


typedef union {
  double literal;
  e_operator_type operator;
  e_bracket_type bracket;
  e_math_constant_type mathConstant;
  e_function_type function;
} u_token_as;

typedef struct {
  e_token_type type;
  u_token_as as;
} token_t;

typedef struct {
  token_t* items;
  size_t capacity;
  size_t count;
  
  bool isError;
} lexer_t;


#define lexer_free(lexer) \
  do {                    \
    da_free(lexer);       \
  } while (0)


#define add_literal_token(lexer, lt)        da_append((lexer), ((token_t) { .type = TT_LITERAL,       .as.literal      = (lt) }))
#define add_operator_token(lexer, op)       da_append((lexer), ((token_t) { .type = TT_OPERATOR,      .as.operator     = (op) }))
#define add_bracket_token(lexer, bt)        da_append((lexer), ((token_t) { .type = TT_BRACKET,       .as.bracket      = (bt) }))
#define add_math_constant_token(lexer, mc)  da_append((lexer), ((token_t) { .type = TT_MATH_CONSTANT, .as.mathConstant = (mc) }))
#define add_function_token(lexer, ft)       da_append((lexer), ((token_t) { .type = TT_FUNCTION,      .as.function     = (ft) }))



void lex_tokens(lexer_t* lexer, token_list_t* tokens)
{
  ASSERT_NULL(lexer);
  ASSERT_NULL(tokens);

  bool isError = false;

  for (size_t i = 0; i < tokens->count; ++i)
  {
    input_token_t* currentToken = &tokens->items[i];
    const char* currentTokenString = currentToken->items;

    if (cstr_is_operator(currentTokenString))
    {
      e_operator_type op = cstr_to_operator_type(currentTokenString);

      if (op == OP_INVALID)
        t_unreachable_defer("Invalid operator-type!");

      add_operator_token(lexer, op);
      continue;
    }
    
    if (cstr_is_bracket(currentTokenString))
    {
      e_bracket_type bt = cstr_to_bracket_type(currentTokenString);

      if (bt == BT_INVALID)
        t_unreachable_defer("Invalid bracket-type!");

      add_bracket_token(lexer, bt);
      continue;
    }

    if (cstr_is_math_constant(currentTokenString))
    {
      e_math_constant_type mc = cstr_to_math_constant_type(currentTokenString);

      if (mc == MC_INVALID)
        l_unreachable_defer("Invalid math-constant-type!");

      add_math_constant_token(lexer, mc);
      continue;
    }

    if (cstr_is_function(currentTokenString))
    {
      e_function_type ft = cstr_to_function_type(currentTokenString);

      if (ft == FT_INVALID)
        l_unreachable_defer("Invalid function-type!");

      add_function_token(lexer, ft);
      continue;
    }
    
    // Literal checking
    lit_check_ret_t literalError = cstr_is_literal(currentTokenString);
    switch (literalError.ret) {
      case 1: l_unreachable_defer("literal was NULL!");
      case 0:
      {
        double literal = strtof(currentTokenString, NULL);
        add_literal_token(lexer, literal);
        continue;
      }
      case -1:
      {
        // Too many commas
        L_ERROR_INVALID_LITERAL(currentToken->cursor + literalError.cursor, currentTokenString);
        isError = true;
        continue;
      }
      case -2: // Not a literal
      default:
        break;
    }

    // ERROR: Invalid token.
    L_ERROR_INVALID_TOKEN(currentToken->cursor, currentTokenString);
    isError = true;
  }

defer:
  lexer->isError = isError;
}


void print_lexed_tokens(lexer_t* lexer)
{
  ASSERT_NULL(lexer);
  
  if (lexer->isError)
  {
    L_ERROR_GIVEN_LEXER_INVALID();
    return;
  }

  printf("Printing lexed tokens ('%zu' tokens found):\n", lexer->count);
  
  for (size_t i = 0; i < lexer->count; ++i) {
    token_t* token = &lexer->items[i];

    printf(tokenTypeNames[token->type]);

    switch (token->type) {
      case TT_LITERAL:
        printf("(%.04f)", token->as.literal);
        break;
      case TT_OPERATOR:
        printf("(%s)", operatorTypeNames[token->as.operator]);
        break;
      case TT_BRACKET:
        printf("(%s)", bracketTypeNames[token->as.bracket]);
        break;
      case TT_MATH_CONSTANT:
        printf("(%s)", mathConstantTypeNames[token->as.mathConstant]);
        break;
      case TT_FUNCTION:
        printf("(%s)", functionTypeNames[token->as.function]);
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
