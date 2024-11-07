// Used for handling the tokenization and lexing of an input math equation.

#ifndef _LEXER_H_
#define _LEXER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "darray.h"
#include "stringslice.h"

// TODO: Maybe put into its own base-header
#define UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

#define t_return_defer() do { isError = true; goto defer; } while (0)

#define COMMA_CHARACTER '.'

#define is_comma(c)     ((c) == '.' || (c) == ',')
#define is_literal(c)   (isdigit(c) || is_comma(c))


#define T_ERROR_INVALID_NUMBER(currPos)              fprintf(stderr, "ERROR:%zu: tokenize: Numbers can only have 1 comma!\n", currPos)
#define T_ERROR_INVALID_CHARACTER(currPos, currChar) fprintf(stderr, "ERROR:%zu: tokenize: '%c' is not an allowed character!\n", currPos, currChar)


typedef enum {
  TT_LITERAL,
  TT_OPERATOR,
  TT_BRACKET,
  TT_FUNCTION,

  TT_COUNT
} e_token_type;

static_assert(TT_COUNT == 4, "Amount of token-types have changed");
static const char* tokenTypeNames[TT_COUNT] = {
	[TT_LITERAL] = "Literal",
	[TT_OPERATOR] = "Operator",
  [TT_BRACKET] = "Bracket",
  [TT_FUNCTION] = "Function"
};


typedef enum {
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,

  OP_COUNT,
  OP_INVALID
} e_operator_type;

static_assert(OP_COUNT == 4, "Amount of operator-types have changed");
static const char* operatorTypeNames[OP_COUNT] = {
	[OP_ADD] = "Add",
	[OP_SUB] = "Subtract",
	[OP_MUL] = "Multiply",
	[OP_DIV] = "Divide",
};

static e_operator_type char_to_operator_type(const char c)
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

#define is_operator(c) (char_to_operator_type(c) != OP_INVALID)


typedef enum {
  BT_OPAREN,
  BT_CPAREN,

  BT_COUNT,
  BT_INVALID
} e_bracket_type;

static_assert(BT_COUNT == 2, "Amount of bracket-types have changed");
static const char* bracketTypeNames[BT_COUNT] = {
  [BT_OPAREN] = "Open-Paren",
  [BT_CPAREN] = "Closing-Paren"
};

static e_bracket_type char_to_bracket_type(const char c)
{
  switch (c)
  {
    case '(': return BT_OPAREN;
    case ')': return BT_CPAREN;
    default:  return BT_INVALID;
  }
}

#define is_bracket(c) (char_to_bracket_type(c) != BT_INVALID)


typedef enum {
  FT_SQRT,
  FT_POW,

  FT_COUNT,
  FT_INVALID
} e_function_type;

static_assert(FT_COUNT == 2, "Amount of function-types have changed");
static const char* functionTypeNames[FT_COUNT] = {
  [FT_SQRT] = "Sqrt",
  [FT_POW] = "Pow"
};


// TODO: Implement function parsing for the tokens.

typedef union {
  double literal;
  e_operator_type operator;
  e_bracket_type bracket;
  e_function_type function;
} u_token_as;

typedef struct {
  e_token_type type;
  u_token_as as;
} token_t;

typedef struct {
  token_t* items;
  size_t count;
  size_t capacity;
  
  bool isError;
} token_list_t;



#define add_literal_token(tokens, lt)   da_append((tokens), ((token_t) { .type = TT_LITERAL,  .as.literal = (lt) }))
#define add_operator_token(tokens, op)  da_append((tokens), ((token_t) { .type = TT_OPERATOR, .as.operator = (op) }))
#define add_bracket_token(tokens, bt)   da_append((tokens), ((token_t) { .type = TT_BRACKET,  .as.bracket = (bt) }))
#define add_function_token(tokens, ft)  da_append((tokens), ((token_t) { .type = TT_FUNCTION, .as.function = (ft) }))



void tokenize(token_list_t* tokens, const char* input, bool debug)
{
  assert(tokens && "'tokens' was NULL!");

  string_slice_t ss = {0};
  string_builder_t cleanedSrcBuff = {0};
  string_builder_t inputBuff = {0};
  bool currentLiteralHasComma = false;
  bool isError = false;

  ss_init(&ss, input);

  if (debug) printf("DEBUG: Input='%s'; Length=%zu\n", ss.src, ss.len);

  // TODO: For proper function parsing this propably must be rewritten to first parse all string tokens and then lex them
  // because currently only character per character gets checked.

  while (ss_in_range(&ss))
  {
    ss_seek_spaces(&ss);
    
    // Checks if the current position is still in range because the input could have
    // spaces at the end and the seek_spaces method could seek to the end.
    if (!ss_in_range(&ss))
      break;

    const char current = ss_get_current(&ss);

    if (is_literal(current))
    {
      if (is_comma(current))
      {
        // ERROR: Invalid literal definition.
        if (currentLiteralHasComma)
        {
          T_ERROR_INVALID_NUMBER(ss_current_pos(&ss));
          t_return_defer();
        }

        currentLiteralHasComma = true;
        sb_append_char(&inputBuff, COMMA_CHARACTER);
      }
      else sb_append_char(&inputBuff, current);

      // Checks for completion of the current literal.
      if ((ss_can_peek(&ss) && !is_literal(ss_peek(&ss))) || !ss_can_peek(&ss))
      {
        double literal = strtof(inputBuff.items, NULL);

        if (debug) printf("DEBUG: > LITERAL FOUND ('%.04f')\n", literal);

        add_literal_token(tokens, literal);

        sb_free(inputBuff);
        currentLiteralHasComma = false;
      }
    }
    else if (is_operator(current))
    {
      e_operator_type op = char_to_operator_type(current);
      if (op == OP_INVALID)
        UNREACHABLE("Invalid operator-type!");

      if (debug) printf("DEBUG: > OPERATOR FOUND ('%c')\n", current);

      add_operator_token(tokens, op);
    }
    else if (is_bracket(current))
    {
      e_bracket_type bt = char_to_bracket_type(current);
      if (bt == BT_INVALID)
        UNREACHABLE("Invalid bracket-type!");

      if (debug) printf("DEBUG: > BRACKET FOUND ('%c')\n", current);

      add_bracket_token(tokens, bt);
    }
    else
    {
      // ERROR: Invalid character.
      T_ERROR_INVALID_CHARACTER(ss_current_pos(&ss), current);
      t_return_defer();
    }

    sb_append_char(&cleanedSrcBuff, current);

    ss_seek(&ss);
  }

  if (debug) printf("DEBUG: Cleaned='%s'; Length=%zu\n", cleanedSrcBuff.items, cleanedSrcBuff.count);

defer:
  sb_free(inputBuff);
  sb_free(cleanedSrcBuff);
  tokens->isError = isError;
}


void print_tokens(token_list_t* tokens)
{
  assert(tokens && "'tokens' was NULL!");

  if (tokens->isError)
  {
    fprintf(stderr, "ERROR: Can't print the tokens because an error happend!\n");
    return;
  }

  printf("Printing tokens ('%zu' tokens found):\n", tokens->count);
  
  for (size_t i = 0; i < tokens->count; ++i) {
    token_t* token = &tokens->items[i];
    size_t tokenIndex = i + 1;

    printf("%zu: %s", tokenIndex, tokenTypeNames[token->type]);

    switch (tokens->items[i].type) {
      case TT_LITERAL:
        printf("(%.04f)", token->as.literal);
        break;
      case TT_OPERATOR:
        printf("(%s)", operatorTypeNames[token->as.operator]);
        break;
      case TT_BRACKET:
        printf("(%s)", bracketTypeNames[token->as.bracket]);
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
