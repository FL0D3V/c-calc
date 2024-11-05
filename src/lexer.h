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


// All types
typedef enum {
  TOKENTYPE_NUMBER,
  TOKENTYPE_COMMA,  // TODO: Rethink! Maybe just implement a different number type for REAL numbers.
  TOKENTYPE_OPERATOR,
  TOKENTYPE_LPARAM,
  TOKENTYPE_RPARAM,

  TOKENTYPE_COUNT
} e_token_type;

typedef enum {
  OPERATION_ADD,
  OPERATION_SUB,
  OPERATION_MUL,
  OPERATION_DIV,

  OPERATION_COUNT
} e_operation;


typedef struct {
  e_token_type type;
  union {
    int literal;
    e_operation operation;
  };
} token_t;


typedef struct {
  token_t* items;
  size_t count;
  size_t capacity;

  char* cleanedSrc;
  size_t cleanedSrcCount;
} token_list_t;


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


// Method definitions

token_list_t tokenize_ex(const char* src, const size_t len, bool debug);

#define tokenize(src, len) tokenize_ex((src), (len), false)

void print_tokens(token_list_t tokens);

#define token_list_free(list)   \
  do {                          \
    da_free(list);              \
    free((list).cleanedSrc);    \
    (list).cleanedSrc = NULL;   \
    (list).cleanedSrcCount = 0; \
  } while(0)


// Method implementations

static size_t seek_spaces(const char* src)
{
  size_t srcStart = (size_t)src;
  while (isspace(*src))
    src++;
  return PTR_DIFF(srcStart, src);
}

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
token_list_t tokenize_ex(const char* src, const size_t len, bool debug)
{
  assert(src && "Given source was NULL!");
  assert(len > 0 && "The src length must be greater than zero!");

  // Reference to the beginning ptr of the src because the src ptr gets incremented
  // and with this it is easy to calculate the current index for e.g. length differences.
  const size_t srcStart = (size_t)src;

  token_list_t tokens = {0};

  string_builder_t digitBuff = {0};
  string_builder_t cleanedSrcBuff = {0};

  if (debug)
    printf("\nInput='%s'; Length=%zu\n", src, len);

  while (PTR_DIFF(srcStart, src) < len)
  {
    // Seek all connected spaces.
    src += seek_spaces(src);

    size_t currPos = PTR_DIFF(srcStart, src);

    if (isdigit(*src))
    {
      if (debug)
        printf("DIGIT FOUND ('%c')\n", *src);
      
      sb_append_char(&digitBuff, *src);

      // Try to peek one forward
      if (PTR_DIFF(srcStart, src) + 1 <= len && !isdigit(*(src + 1)))
      {
        if (debug)
          printf("LITERAL COMPLETE ('%s')\n", digitBuff.items);
        
        token_t token = { .type = TOKENTYPE_NUMBER, .literal = atoi(digitBuff.items) };
        da_append(&tokens, token);

        sb_free(digitBuff);
      }
    }
    else if (is_comma(*src))
    {
      if (debug)
        printf("COMMA FOUND ('%c')\n", *src);
      
      token_t token = { .type = TOKENTYPE_COMMA };
      da_append(&tokens, token);
    }
    else if (is_operation(*src))
    {
      if (debug)
        printf("OPERATION FOUND ('%c')\n", *src);

      int operation = char_to_operation(*src);
      assert(operation >= 0 && "This should not happen!");

      token_t token = { .type = TOKENTYPE_OPERATOR, .operation = (e_operation)operation };
      da_append(&tokens, token);
    }
    else if (is_brace(*src))
    {
      if (debug)
        printf("BRACE FOUND ('%c')\n", *src);

      int braceType = brace_to_token_type(*src);
      assert(braceType >= 0 && "This should not happen!");

      token_t token = { .type = (e_token_type)braceType };
      da_append(&tokens, token);
    }
    else
    {
      // ERROR CHECK
      if (iscntrl(*src))
        printf("ERROR:%zu: tokenize: Control characters are not allowed!\n", currPos);
      else
        printf("ERROR:%zu: tokenize: '%c' is not an allowed character!\n", currPos, *src);
      
      da_free(tokens);
      return tokens;
    }

    sb_append_char(&cleanedSrcBuff, *src);

    src++;
  }

  if (debug)
    printf("Cleaned='%s'; Length=%zu\n", cleanedSrcBuff.items, cleanedSrcBuff.count);

  tokens.cleanedSrc = cleanedSrcBuff.items;
  tokens.cleanedSrcCount = cleanedSrcBuff.count;
  //sb_free(cleanedSrcBuff);

  return tokens;
}


void print_tokens(token_list_t tokens)
{
  if (!tokens.items)
  {
    printf("ERROR: There was an error while tokenizing so there is nothing to print!\n");
    return;
  }

  printf("\nPrinting tokens ('%zu' tokens found):\n", tokens.count);
  printf("Original cleaned input is:\n");
  printf("%s\n\n", tokens.cleanedSrc);

  for (size_t i = 0; i < tokens.count; ++i) {
    printf("Type=%s", tokenTypeNames[tokens.items[i].type]);
    if (tokens.items[i].type == TOKENTYPE_NUMBER)
      printf("; Literal=%d", tokens.items[i].literal);
    else if (tokens.items[i].type == TOKENTYPE_OPERATOR)
      printf("; Operation=%s", operationNames[tokens.items[i].operation]);
    printf("\n");
  }
}

#endif // _LEXER_H_
