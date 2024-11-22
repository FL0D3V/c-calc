// Used for handling the tokenization and lexing of an input math equation.

#ifndef _LEXER_H_
#define _LEXER_H_

#include "global.h"
#include "darray.h"
#include "stringslice.h"
#include "tokenizer.h"


#define l_return_defer()              do { isError = true; goto defer; } while (0)
#define l_unreachable_defer(message)  do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); l_return_defer(); } while(0)


#define L_ERROR_NAME "lexing_error"
#define L_ERROR_INVALID_NUMBER(cursor, cstr)  fprintf(stderr, L_ERROR_NAME ":%zu: A number can only contain 1 comma ('%s')!\n", (cursor), (cstr))
#define L_ERROR_INVALID_TOKEN(cursor, cstr)   fprintf(stderr, L_ERROR_NAME ":%zu: '%s' is an invalid token!\n", (cursor), (cstr))
#define L_ERROR_GIVEN_LEXER_INVALID()         fprintf(stderr, L_ERROR_NAME ": Can't print the lexer because an error happend!\n")


// TODO: Implement variables and variable assigning
// Variables could look like e.g.: 'a', 'b', 'out1'
// Variable assigning could look like: ':='
// Also 'solve' could be a function which allows for using the '=' operator inside and variables for more complex expressions

typedef enum {
  TT_NUMBER,
  TT_MATH_CONSTANT,
  TT_OPERATOR,
  TT_PAREN,
  TT_FUNCTION,

  TT_COUNT
} e_token_type;

static_assert(TT_COUNT == 5, "Amount of token-types have changed");
const char* tokenTypeNames[TT_COUNT] = {
	[TT_NUMBER] = "Number",
  [TT_MATH_CONSTANT] = "Constant",
  [TT_OPERATOR] = "Operator",
  [TT_PAREN] = "Parenthesis",
  [TT_FUNCTION] = "Function",
};


typedef union {
  double number;
  e_math_constant_type constant;
  e_operator_type operator;
  e_paren_type paren;
  e_function_type function;
} u_token_as;

typedef struct {
  e_token_type type;
  size_t cursor;
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


#define add_number_token(lexer, num, curr)        da_append((lexer), ((token_t) { .type = TT_NUMBER,        .as.number   = (num), .cursor = (curr) }))
#define add_math_constant_token(lexer, mc, curr)  da_append((lexer), ((token_t) { .type = TT_MATH_CONSTANT, .as.constant = (mc),  .cursor = (curr) }))
#define add_operator_token(lexer, op, curr)       da_append((lexer), ((token_t) { .type = TT_OPERATOR,      .as.operator = (op),  .cursor = (curr) }))
#define add_paren_token(lexer, pt, curr)          da_append((lexer), ((token_t) { .type = TT_PAREN,         .as.paren    = (pt),  .cursor = (curr) }))
#define add_function_token(lexer, ft, curr)       da_append((lexer), ((token_t) { .type = TT_FUNCTION,      .as.function = (ft),  .cursor = (curr) }))



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

      add_operator_token(lexer, op, currentToken->cursor);
      continue;
    }
    
    if (cstr_is_paren(currentTokenString))
    {
      e_paren_type pt = cstr_to_paren_type(currentTokenString);

      if (pt == PT_INVALID)
        t_unreachable_defer("Invalid paren-type!");

      add_paren_token(lexer, pt, currentToken->cursor);
      continue;
    }

    if (cstr_is_math_constant(currentTokenString))
    {
      e_math_constant_type mc = cstr_to_math_constant_type(currentTokenString);

      if (mc == MC_INVALID)
        l_unreachable_defer("Invalid math-constant-type!");

      add_math_constant_token(lexer, mc, currentToken->cursor);
      continue;
    }

    if (cstr_is_function(currentTokenString))
    {
      e_function_type ft = cstr_to_function_type(currentTokenString);

      if (ft == FT_INVALID)
        l_unreachable_defer("Invalid function-type!");

      add_function_token(lexer, ft, currentToken->cursor);
      continue;
    }
    
    // Number checking
    num_check_t numCheck = cstr_is_number(currentTokenString);

    switch (numCheck.ret) {
      case 1: l_unreachable_defer("Number-Token was NULL!");
      case 0:
      {
        double number = strtof(currentTokenString, NULL);
        add_number_token(lexer, number, currentToken->cursor);
        continue;
      }
      case -1:
      {
        // Too many commas
        L_ERROR_INVALID_NUMBER(currentToken->cursor + numCheck.cursor, currentTokenString);
        isError = true;
        continue;
      }
      case -2: // Not a number
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

  printf("Printing lexed tokens ('%zu' tokens):\n", lexer->count);
  
  for (size_t i = 0; i < lexer->count; ++i) {
    token_t* token = &lexer->items[i];

    printf(tokenTypeNames[token->type]);

    switch (token->type) {
      case TT_NUMBER:
        printf("(" DOUBLE_PRINT_FORMAT ")", token->as.number);
        break;
      case TT_MATH_CONSTANT:
        printf("(" DOUBLE_PRINT_FORMAT ", %s)", mathConstantTypeValues[token->as.constant], mathConstantTypeNames[token->as.constant]);
        break;
      case TT_OPERATOR:
        printf("(%s)", operatorTypeNames[token->as.operator]);
        break;
      case TT_PAREN:
        printf("(%s)", parenTypeNames[token->as.paren]);
        break;
      case TT_FUNCTION:
        printf("(%s, %s)", functionTypeIdentifiers[token->as.function], functionTypeNames[token->as.function]);
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
