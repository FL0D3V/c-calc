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


typedef enum {
  TOKENTYPE_NUMBER,

  // TODO: Rethink! Maybe just implement a different number type for REAL numbers.
  TOKENTYPE_COMMA,
  
  TOKENTYPE_OPERATOR,

  // TODO: Rethink if the braces should be combined in one token-type and the token should store in the union a new type of the brace (a new enum)
  TOKENTYPE_LPARAM,
  TOKENTYPE_RPARAM,

  TOKENTYPE_COUNT
} e_token_type;

// TODO: Add more opeartors.
typedef enum {
  OPERATION_ADD,
  OPERATION_SUB,
  OPERATION_MUL,
  OPERATION_DIV,

  OPERATION_COUNT
} e_operation;


// OLD:
/*typedef struct {
  e_token_type type;
  union {
    int literal;
    e_operation operation;
  };
} token_t;*/


// The token implementation with all variations.
// TODO: Implement different types of numbers (int, float)
typedef struct {
  e_token_type type;
  union {
    int literal;
    e_operation operation;
  };
} token_t;


// This is a dynamic array with extras also the cleaned source for pretty printing.
typedef struct {
  token_t* items;
  size_t count;
  size_t capacity;

  char* cleanedSrc;
  size_t cleanedSrcCount;
} token_list_t;


#define token_list_free(list)   \
  do {                          \
    da_free(list);              \
    free((list).cleanedSrc);    \
    (list).cleanedSrc = NULL;   \
    (list).cleanedSrcCount = 0; \
  } while(0)



// Enum to string conversion lists
static const char* tokenTypeNames[TOKENTYPE_COUNT] = {
	[TOKENTYPE_NUMBER] = "Number",
	[TOKENTYPE_COMMA] = "Comma",
	[TOKENTYPE_OPERATOR] = "Operator",
	[TOKENTYPE_LPARAM] = "OpenBrace",
	[TOKENTYPE_RPARAM] = "ClosingBrace"
};

static const char operationChars[OPERATION_COUNT] = {
  [OPERATION_ADD] = '+',
	[OPERATION_SUB] = '-',
	[OPERATION_MUL] = '*',
	[OPERATION_DIV] = '/'
};

static const char* operationNames[OPERATION_COUNT] = {
	[OPERATION_ADD] = "Add",
	[OPERATION_SUB] = "Subtract",
	[OPERATION_MUL] = "Multiply",
	[OPERATION_DIV] = "Divide"
};



// Util functions

static bool is_comma(const char c)
{
  return c == '.' || c == ',';
}

static bool is_brace(const char c)
{
  return c == '(' || c == ')';
}

static int brace_to_token_type(const char c)
{
  if (c == '(')
    return TOKENTYPE_LPARAM;
  else if (c == ')')
    return TOKENTYPE_RPARAM;
  else
    return -1;
}

static bool is_operation(const char c)
{
  for (int i = 0; i < OPERATION_COUNT; ++i)
    if (operationChars[i] == c)
      return true;
  return false;
}

static int char_to_operation(const char c)
{
  for (int i = 0; i < OPERATION_COUNT; ++i)
    if (operationChars[i] == c)
      return (e_operation) i;
  return -1;
}


// Tokenizes and lexes a given string.
// Example:
//  In the output every line would represent a single token.
//  Input:
//    "10.5 + 2 * (7.2 - 5) / 6"
//  Output:
//    Number; Literal=10
//    Comma
//    Number; Literal=5
//    Operator; Operation=Add
//    Number; Literal=2
//    Operator; Operation=Multiply
//    OpenBrace
//    Number; Literal=7
//    Comma
//    Number; Literal=2
//    Operator; Operation=Subtract
//    Number; Literal=5
//    ClosingBrace
//    Operator; Operation=Divide
//    Number; Literal=6
token_list_t tokenize(const char* input, bool debug)
{
  string_slice_t ss = {0};
  ss_init(&ss, input);

  token_list_t tokens = {0};
  string_builder_t digitBuff = {0};
  string_builder_t cleanedSrcBuff = {0};

  if (debug)
    printf("DEBUG: Input='%s'; Length=%zu\n", ss.src, ss.len);

  while (ss_in_range(&ss))
  {
    ss_seek_spaces(&ss);
    
    if (!ss_in_range(&ss))
      break;

    const char current = ss_get_current(&ss);

    if (isdigit(current))
    {
      if (debug)
        printf("DEBUG: > DIGIT FOUND ('%c')\n", current);
      
      sb_append_char(&digitBuff, current);

      // Checks if the next is NOT a digit or if the end was reached.
      if ((ss_can_peek(&ss) && !isdigit(ss_peek(&ss))) || !ss_can_peek(&ss))
      {
        if (debug)
          printf("DEBUG: > LITERAL COMPLETE ('%s')\n", digitBuff.items);
        
        da_append(&tokens, ((token_t) { .type = TOKENTYPE_NUMBER, .literal = atoi(digitBuff.items) }));
        sb_free(digitBuff);
      }
    }
    else if (is_comma(current))
    {
      if (debug)
        printf("DEBUG: > COMMA FOUND ('%c')\n", current);
      
      da_append(&tokens, ((token_t) { .type = TOKENTYPE_COMMA }));
    }
    else if (is_operation(current))
    {
      if (debug)
        printf("DEBUG: > OPERATION FOUND ('%c')\n", current);

      int operation = char_to_operation(current);
      assert(operation >= 0 && "This should not happen!");

      da_append(&tokens, ((token_t) { .type = TOKENTYPE_OPERATOR, .operation = (e_operation)operation }));
    }
    else if (is_brace(current))
    {
      if (debug)
        printf("DEBUG: > BRACE FOUND ('%c')\n", current);

      int braceType = brace_to_token_type(current);
      assert(braceType >= 0 && "This should not happen!");

      da_append(&tokens, ((token_t) { .type = (e_token_type)braceType }));
    }
    else
    {
      if (iscntrl(current))
        printf("ERROR:%zu: tokenize: Control characters are not allowed!\n", ss_current_pos(&ss));
      else
        printf("ERROR:%zu: tokenize: '%c' is not an allowed character!\n", ss_current_pos(&ss), current);
      
      da_free(tokens);
      return tokens;
    }

    sb_append_char(&cleanedSrcBuff, current);

    ss_seek(&ss);
  }

  if (debug)
    printf("DEBUG: Cleaned='%s'; Length=%zu\n", cleanedSrcBuff.items, cleanedSrcBuff.count);

  tokens.cleanedSrc = cleanedSrcBuff.items;
  tokens.cleanedSrcCount = cleanedSrcBuff.count;

  return tokens;
}


void print_tokens(token_list_t tokens)
{
  if (!tokens.items)
  {
    printf("ERROR: There was an error while tokenizing so there is nothing to print!\n");
    return;
  }

  printf("Printing tokens ('%zu' tokens found):\n", tokens.count);

  for (size_t i = 0; i < tokens.count; ++i) {
    printf("%zu: Type=%s", i + 1, tokenTypeNames[tokens.items[i].type]);
    if (tokens.items[i].type == TOKENTYPE_NUMBER)
      printf("; Literal=%d", tokens.items[i].literal);
    else if (tokens.items[i].type == TOKENTYPE_OPERATOR)
      printf("; Operation=%s", operationNames[tokens.items[i].operation]);
    printf("\n");
  }
}

#endif // _LEXER_H_
