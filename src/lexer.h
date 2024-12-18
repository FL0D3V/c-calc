#ifndef _LEXER_H_
#define _LEXER_H_

// Everything else needed is included in the tokenizer.
#include "tokenizer.h"


// Error handling
#define L_ERROR_NAME "LEXING-ERROR"
#define L_ERROR_INVALID_NUMBER(cursor, tok) fprintf(stderr, L_ERROR_NAME ":%zu: A number can only contain 1 comma ('" IN_TOK_FMT "')!\n", (cursor), IN_TOK_ARG(tok))
#define L_ERROR_INVALID_TOKEN(cursor, tok)  fprintf(stderr, L_ERROR_NAME ":%zu: '" IN_TOK_FMT "' is an invalid token!\n", (cursor), IN_TOK_ARG(tok))
#define L_ERROR_GIVEN_LEXER_INVALID()       fprintf(stderr, L_ERROR_NAME ": Can't print the lexer because an error happend!\n")



// TODO: Implement variables and variable assigning
// Variables could look like e.g.: 'a', 'b', 'out1'
// Variable assigning could look like: ':=' or '='
// Also 'solve' could be a function which allows for using the '=' literal inside and variables for more complex expressions and equations.


// All enums
typedef enum {
  TT_NUMBER,
  TT_MATH_CONSTANT,
  TT_OPERATOR,
  TT_PAREN,
  TT_FUNCTION,
  TT_LITERAL, // Here are all not connected literals like ',' or '='.

  TT_COUNT
} e_token_type;

static_assert(TT_COUNT == 6, "Amount of token-types have changed");

static const char* tokenTypeNames[TT_COUNT] = {
	[TT_NUMBER] = "Number",
  [TT_MATH_CONSTANT] = "Constant",
  [TT_OPERATOR] = "Operator",
  [TT_PAREN] = "Parenthesis",
  [TT_FUNCTION] = "Function",
  [TT_LITERAL] = "Literal",
};


// Type-Definitions
typedef union {
  double number;
  e_math_constant_type constant;
  e_operator_type operator;
  e_paren_type paren;
  e_function_type function;
  e_common_literal_type literal;
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


#define lexer_free(lexer) da_free(lexer)

// TODO: Change to 'arena_da_append' and use a single arena allocator for all allocations.
#define add_number_token(lexer, num, curr)        da_append((lexer), ((token_t) { .type = TT_NUMBER,        .as.number   = (num), .cursor = (curr) }))
#define add_math_constant_token(lexer, mc, curr)  da_append((lexer), ((token_t) { .type = TT_MATH_CONSTANT, .as.constant = (mc),  .cursor = (curr) }))
#define add_operator_token(lexer, op, curr)       da_append((lexer), ((token_t) { .type = TT_OPERATOR,      .as.operator = (op),  .cursor = (curr) }))
#define add_paren_token(lexer, pt, curr)          da_append((lexer), ((token_t) { .type = TT_PAREN,         .as.paren    = (pt),  .cursor = (curr) }))
#define add_function_token(lexer, ft, curr)       da_append((lexer), ((token_t) { .type = TT_FUNCTION,      .as.function = (ft),  .cursor = (curr) }))
#define add_literal_token(lexer, clt, curr)       da_append((lexer), ((token_t) { .type = TT_LITERAL,       .as.literal =  (clt), .cursor = (curr) }))


// Helpers
#define lex_at(lexer, idx)            (&(lexer)->items[(idx)])
#define lex_next_in_range(lexer, idx) ((idx) < (lexer)->count - 1)
#define tok_is(tok, t)                ((tok)->type == (t))
#define tok_not(tok, t)               ((tok)->type != (t))


lexer_t lexer_execute(tokenizer_t* tokenizer)
{
  ASSERT_NULL(tokenizer);

  lexer_t lexer = {0};

  for (size_t i = 0; i < tokenizer->count; ++i)
  {
    const input_token_t* currentToken = &tokenizer->items[i];

    // Operator needs to be checked at first because else the double conversion fails on number parsing.
    if (cstr_is_operator_ex(currentToken->value, currentToken->length))
    {
      e_operator_type op = cstr_to_operator_type_ex(currentToken->value, currentToken->length);

      if (op == OP_INVALID)
        UNREACHABLE("Invalid operator-type!");

      add_operator_token(&lexer, op, currentToken->cursor);
      continue;
    }


    // Number checking
    num_check_t numCheck = cstr_is_number_ex(currentToken->value, currentToken->length);

    if (numCheck.ret == 1) UNREACHABLE("Number-Token was NULL!");
    else if (numCheck.ret == 0)
    {
      char* endptr;
      double number = strtof(currentToken->value, &endptr);
      
      if (!endptr || endptr == currentToken->value || (size_t)(endptr - currentToken->value) != currentToken->length)
        UNREACHABLE("Error while converting a number!");

      add_number_token(&lexer, number, currentToken->cursor);
      continue;
    }
    else if (numCheck.ret == -1)
    {
      // Too many commas
      L_ERROR_INVALID_NUMBER(currentToken->cursor + numCheck.cursor, currentToken);
      lexer.isError = true;
      continue;
    }
    // Else: Not a number.


    if (cstr_is_paren_ex(currentToken->value, currentToken->length))
    {
      e_paren_type pt = cstr_to_paren_type_ex(currentToken->value, currentToken->length);

      if (pt == PT_INVALID)
        UNREACHABLE("Invalid paren-type!");

      add_paren_token(&lexer, pt, currentToken->cursor);
      continue;
    }


    if (cstr_is_math_constant_ex(currentToken->value, currentToken->length))
    {
      e_math_constant_type mc = cstr_to_math_constant_type_ex(currentToken->value, currentToken->length);

      if (mc == MC_INVALID)
        UNREACHABLE("Invalid math-constant-type!");

      add_math_constant_token(&lexer, mc, currentToken->cursor);
      continue;
    }


    if (cstr_is_function_ex(currentToken->value, currentToken->length))
    {
      e_function_type ft = cstr_to_function_type_ex(currentToken->value, currentToken->length);

      if (ft == FT_INVALID)
        UNREACHABLE("Invalid function-type!");

      add_function_token(&lexer, ft, currentToken->cursor);
      continue;
    }


    if (cstr_is_common_literal_ex(currentToken->value, currentToken->length))
    {
      e_common_literal_type clt = cstr_to_common_literal_type_ex(currentToken->value, currentToken->length);

      if (clt == CLT_INVALID)
        UNREACHABLE("Invalid common-literal-type!");

      add_literal_token(&lexer, clt, currentToken->cursor);
      continue;
    }

    
    // ERROR: Invalid token.
    L_ERROR_INVALID_TOKEN(currentToken->cursor, currentToken);
    lexer.isError = true;
  }

  return lexer;
}


void lexer_print(const lexer_t* lexer)
{
  ASSERT_NULL(lexer);
  
  if (lexer->isError || lexer->count == 0)
  {
    L_ERROR_GIVEN_LEXER_INVALID();
    return;
  }

  printf("Printing lexed tokens (%zu tokens):\n", lexer->count);
  
  for (size_t i = 0; i < lexer->count; ++i) {
    token_t* token = &lexer->items[i];

    printf("%s", tokenTypeNames[token->type]);

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
      case TT_LITERAL:
        printf("(%s)", commonLiteralTypeNames[token->as.literal]);
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
