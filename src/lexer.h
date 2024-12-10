// Used for handling the tokenization and lexing of an input math equation.

#ifndef _LEXER_H_
#define _LEXER_H_

#include "global.h"
#include "darray.h"
#include "stringslice.h"
#include "helpers.h"
#include "tokenizer.h"


#define l_unreachable_defer(message)  do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); goto defer; } while(0)


#define L_ERROR_NAME "LEXING-ERROR"
#define L_ERROR_INVALID_NUMBER(cursor, cstr)  fprintf(stderr, L_ERROR_NAME ":%zu: A number can only contain 1 comma ('%s')!\n", (cursor), (cstr))
#define L_ERROR_INVALID_TOKEN(cursor, cstr)   fprintf(stderr, L_ERROR_NAME ":%zu: '%s' is an invalid token!\n", (cursor), (cstr))
#define L_ERROR_GIVEN_LEXER_INVALID()         fprintf(stderr, L_ERROR_NAME ": Can't print the lexer because an error happend!\n")


// TODO: Implement variables and variable assigning
// Variables could look like e.g.: 'a', 'b', 'out1'
// Variable assigning could look like: ':=' or '='
// Also 'solve' could be a function which allows for using the '=' literal inside and variables for more complex expressions

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
const char* tokenTypeNames[TT_COUNT] = {
	[TT_NUMBER] = "Number",
  [TT_MATH_CONSTANT] = "Constant",
  [TT_OPERATOR] = "Operator",
  [TT_PAREN] = "Parenthesis",
  [TT_FUNCTION] = "Function",
  [TT_LITERAL] = "Literal",
};


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

#define lexer_free(lexer) da_free(lexer);


#define tok_is(tok, t)        ((tok)->type == (t))
#define tok_not(tok, t)       ((tok)->type != (t))

#define tok_is_paren(tok, pt)           (tok_is(tok, TT_PAREN) && (tok)->as.paren == (pt))
#define tok_not_specific_paren(tok, pt) (tok_not((tok), TT_PAREN) || (tok)->as.paren != (pt))
#define tok_is_number_operator(tok)     (tok_is(tok, TT_OPERATOR) && ((tok)->as.operator == OP_ADD || (tok)->as.operator == OP_SUB))

#define lex_at(lexer, idx) (&(lexer)->items[(idx)])
#define lex_next_in_range(lexer, idx) ((idx) < (lexer)->count - 1)


#define add_number_token(lexer, num, curr)        da_append((lexer), ((token_t) { .type = TT_NUMBER,        .as.number   = (num), .cursor = (curr) }))
#define add_math_constant_token(lexer, mc, curr)  da_append((lexer), ((token_t) { .type = TT_MATH_CONSTANT, .as.constant = (mc),  .cursor = (curr) }))
#define add_operator_token(lexer, op, curr)       da_append((lexer), ((token_t) { .type = TT_OPERATOR,      .as.operator = (op),  .cursor = (curr) }))
#define add_paren_token(lexer, pt, curr)          da_append((lexer), ((token_t) { .type = TT_PAREN,         .as.paren    = (pt),  .cursor = (curr) }))
#define add_function_token(lexer, ft, curr)       da_append((lexer), ((token_t) { .type = TT_FUNCTION,      .as.function = (ft),  .cursor = (curr) }))
#define add_literal_token(lexer, clt, curr)       da_append((lexer), ((token_t) { .type = TT_LITERAL,       .as.literal =  (clt), .cursor = (curr) }))


lexer_t lexer_execute(tokenizer_t* tokens)
{
  ASSERT_NULL(tokens);

  lexer_t lexer = {0};
  string_builder_t sb = {0};

  for (size_t i = 0; i < tokens->count; ++i)
  {
    const input_token_t* currentToken = &tokens->items[i];
    sb_clear(&sb);
    sb_append_buf_with_null_termination(&sb, currentToken->value, currentToken->length);
    const char* currentTokenString = sb.items;

    if (cstr_is_operator(currentTokenString))
    {
      e_operator_type op = cstr_to_operator_type(currentTokenString);

      if (op == OP_INVALID)
        l_unreachable_defer("Invalid operator-type!");

      add_operator_token(&lexer, op, currentToken->cursor);
      continue;
    }
    
    if (cstr_is_paren(currentTokenString))
    {
      e_paren_type pt = cstr_to_paren_type(currentTokenString);

      if (pt == PT_INVALID)
        l_unreachable_defer("Invalid paren-type!");

      add_paren_token(&lexer, pt, currentToken->cursor);
      continue;
    }

    if (cstr_is_math_constant(currentTokenString))
    {
      e_math_constant_type mc = cstr_to_math_constant_type(currentTokenString);

      if (mc == MC_INVALID)
        l_unreachable_defer("Invalid math-constant-type!");

      add_math_constant_token(&lexer, mc, currentToken->cursor);
      continue;
    }

    if (cstr_is_function(currentTokenString))
    {
      e_function_type ft = cstr_to_function_type(currentTokenString);

      if (ft == FT_INVALID)
        l_unreachable_defer("Invalid function-type!");

      add_function_token(&lexer, ft, currentToken->cursor);
      continue;
    }

    if (cstr_is_common_literal(currentTokenString))
    {
      e_common_literal_type clt = cstr_to_common_literal_type(currentTokenString);

      if (clt == CLT_INVALID)
        l_unreachable_defer("Invalid common-literal-type!");

      add_literal_token(&lexer, clt, currentToken->cursor);
      continue;
    }
    
    // Number checking
    num_check_t numCheck = cstr_is_number(currentTokenString);

    switch (numCheck.ret) {
      case 1: l_unreachable_defer("Number-Token was NULL!");
      case 0:
      {
        double number = strtof(currentTokenString, NULL);
        add_number_token(&lexer, number, currentToken->cursor);
        continue;
      }
      case -1:
      {
        // Too many commas
        L_ERROR_INVALID_NUMBER(currentToken->cursor + numCheck.cursor, currentTokenString);
        lexer.isError = true;
        continue;
      }
      case -2: // Not a number
      default:
        break;
    }

    // ERROR: Invalid token.
    L_ERROR_INVALID_TOKEN(currentToken->cursor, currentTokenString);
    lexer.isError = true;
  }

defer:
  sb_free(sb);

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
