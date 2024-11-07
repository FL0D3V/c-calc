// Used for handling the tokenization and lexing of an input math equation.

#ifndef _LEXER_H_
#define _LEXER_H_

#include "global.h"
#include "darray.h"
#include "stringslice.h"

#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define t_unreachable_defer(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); t_return_defer(); } while(0)
#define t_return_defer() do { isError = true; goto defer; } while (0)

#define COMMA_CHARACTER '.'

#define is_comma(c)     ((c) == '.' || (c) == ',')
#define is_literal(c)   (isdigit(c) || is_comma(c))


#define T_ERROR_INVALID_NUMBER(currPos)              fprintf(stderr, "lexer_error:%zu: Numbers can only have 1 comma!\n", currPos)
#define T_ERROR_INVALID_CHARACTER(currPos, currChar) fprintf(stderr, "lexer_error:%zu: '%c' is not an allowed character!\n", currPos, currChar)
#define T_ERROR_INVALID_LEXER()                      fprintf(stderr, "lexer_error: Can't print the lexer because an error happend!\n")


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


typedef struct {
  const char** items;
  size_t capacity;
  size_t count;
} token_list_t;

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
  size_t capacity;
  size_t count;
  
  bool isError;
} lexer_t;



#define token_list_free(tokens) da_free(tokens)

#define lexer_free(lexer) \
  do {                    \
    da_free(lexer);       \
  } while (0)



#define add_literal_token(lexer, lt)   da_append((lexer), ((token_t) { .type = TT_LITERAL,  .as.literal = (lt) }))
#define add_operator_token(lexer, op)  da_append((lexer), ((token_t) { .type = TT_OPERATOR, .as.operator = (op) }))
#define add_bracket_token(lexer, bt)   da_append((lexer), ((token_t) { .type = TT_BRACKET,  .as.bracket = (bt) }))
#define add_function_token(lexer, ft)  da_append((lexer), ((token_t) { .type = TT_FUNCTION, .as.function = (ft) }))



void lexer_tokenize(lexer_t* lexer, const char* input, bool debug)
{
  ASSERT_NULL(lexer);

  string_slice_t ss = {0};
  string_builder_t cleanedSrcBuff = {0};
  string_builder_t inputBuff = {0};
  bool currentLiteralHasComma = false;
  bool isError = false;

  ss_init(&ss, input);

  if (debug)
    printf("DEBUG: Input='%s'; Length=%zu\n", ss.src, ss.len);

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

        add_literal_token(lexer, literal);

        sb_free(inputBuff);
        currentLiteralHasComma = false;
      }
    }
    else if (is_operator(current))
    {
      e_operator_type op = char_to_operator_type(current);

      if (op == OP_INVALID)
        t_unreachable_defer("Invalid operator-type!");

      add_operator_token(lexer, op);
    }
    else if (is_bracket(current))
    {
      e_bracket_type bt = char_to_bracket_type(current);

      if (bt == BT_INVALID)
        t_unreachable_defer("Invalid bracket-type!");

      add_bracket_token(lexer, bt);
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

  if (debug)
    printf("DEBUG: Cleaned='%s'; Length=%zu\n", cleanedSrcBuff.items, cleanedSrcBuff.count);

defer:
  sb_free(inputBuff);
  sb_free(cleanedSrcBuff);
  lexer->isError = isError;
}


void lexer_print(lexer_t* lexer)
{
  ASSERT_NULL(lexer);
  
  if (lexer->isError)
  {
    T_ERROR_INVALID_LEXER();
    return;
  }

  printf("Printing tokens ('%zu' tokens found):\n", lexer->count);
  
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
