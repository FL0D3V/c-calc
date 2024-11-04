/*
 * The lexer is used for converting the input text into usable tokens.
 */

#ifndef _LEXER_H_
#define _LEXER_H_

#define ARRLEN(src) (sizeof(src) / sizeof((src)[0]))

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>


// TODO: Implement decimal numbers!
typedef enum {
  TOKENTYPE_NUMBER,
  TOKENTYPE_OPERATOR,
  TOKENTYPE_LPARAM,
  TOKENTYPE_RPARAM,

  TOKENTYPE_COUNT
} e_token_type;

static const char* tokenTypeNames[TOKENTYPE_COUNT] = {
	[TOKENTYPE_NUMBER] = "Number",
	[TOKENTYPE_OPERATOR] = "Operator",
	[TOKENTYPE_LPARAM] = "(",
	[TOKENTYPE_RPARAM] = ")"
};


typedef enum {
  OPERATION_ADD,
  OPERATION_SUB,
  OPERATION_MUL,
  OPERATION_DIV,

  OPERATION_COUNT
} e_operation;

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


static bool is_brace(const char c) {
  return c == '(' || c == ')';
}

static bool is_op(const char c) {
  for (int i = 0; i < OPERATION_COUNT; ++i)
    if (operationChars[i] == c)
      return true;
  return false;
}

static int char_to_op(const char c) {
  for (int i = 0; i < OPERATION_COUNT; ++i)
    if (operationChars[i] == c)
      return (e_operation) i;
  return -1;
}


typedef struct {
  e_token_type type;
  union {
    // TODO: Implement decimals!
    int literal;
    e_operation operation;
  };
} token_t;


typedef struct {
  token_t* buff;
  size_t count;
  const char* error;
} tokenizer_out_t;


/*
enum {
  NODETYPE_LITERAL,
  NODETYPE_BIN_OP,

  NODETYPE_COUNT
} e_node_type;

typedef struct syntax_node {
  e_node_type nodeType;
  union {
    size_t literal;
    struct {
      e_operation operation;
      struct syntax_node* left;
      struct syntax_node* right;
    } bin_op;
  };
} syntax_node_t;
*/


#define TOKEN_COUNT 1024
#define TOKEN_BUFFER_COUNT 256
#define MAX_ERROR_LEN 512


// Tokenizes a given string like: "100 + (4 * 6) / 8"
tokenizer_out_t tokenize(const char* src, size_t len)
{
  assert(src && "Given source was NULL!");

  token_t tokens[TOKEN_COUNT] = {0};
  size_t currentTokenIndex = 0;

  size_t currPos = 0;
  char error[MAX_ERROR_LEN] = {0};

  char buff[TOKEN_BUFFER_COUNT] = {0};
  size_t currentDigitIndex = 0;

  while (currPos < len) {
    // ERROR CHECK
    if (iscntrl(*src)) {
      sprintf(error, "ERROR:%zu: tokenize: Control characters are not allowed!\n", currPos);
      return (tokenizer_out_t) { .buff = NULL, .count = 0, .error = error };
    }

    // ERROR CHECK
    if (currentTokenIndex + 1 >= ARRLEN(tokens)) {
      sprintf(error, "ERROR:%zu: tokenize: Too many tokens! Max number: '%zu'\n", currPos, ARRLEN(tokens));
      return (tokenizer_out_t) { .buff = NULL, .count = 0, .error = error };
    }


    if (!isspace(*src)) {
      // ERROR CHECK
      if (!isdigit(*src) &&
          !is_op(*src) &&
          !is_brace(*src))
      {
        sprintf(error, "ERROR:%zu: tokenize: '%c' is a forbidden character!\n", currPos, *src);
        return (tokenizer_out_t) { .buff = NULL, .count = 0, .error = error };
      }

      // ERROR CHECK
      if (currentDigitIndex + 1 >= ARRLEN(buff)) {
        sprintf(error, "ERROR:%zu: tokenize: Current token was too long! Value: '%s'; Max-length: '%zu'\n", currPos, buff, ARRLEN(buff));
        return (tokenizer_out_t) { .buff = NULL, .count = 0, .error = error };
      }

      if (isdigit(*src))
        buff[currentDigitIndex++] = *src;
      
      if (currPos + 1 < len && !isdigit(*(src + 1))) {
        tokens[currentTokenIndex++] = (token_t) { .type = TOKENTYPE_NUMBER, .literal = atoi(buff) };
        
        memset(buff, '\0', sizeof(buff));
        currentDigitIndex = 0;
      }

      if (is_op(*src))
        tokens[currentTokenIndex++] = (token_t) { .type = TOKENTYPE_OPERATOR, .operation = (e_operation)char_to_op(*src) };
      
      if (*src == '(')
        tokens[currentTokenIndex++] = (token_t) { .type = TOKENTYPE_LPARAM };
      
      if (*src == ')')
        tokens[currentTokenIndex++] = (token_t) { .type = TOKENTYPE_RPARAM };
    }
    
    ++src;
    ++currPos;
  }

  token_t* finalTokens = (token_t*) malloc(sizeof(token_t) * currentTokenIndex);
  assert(finalTokens && "Out of memory!\n");

  memcpy(finalTokens, tokens, currentTokenIndex);

  return (tokenizer_out_t) { .buff = finalTokens, .count = currentTokenIndex, .error = NULL };
}


void print_tokens(tokenizer_out_t tokens)
{
  assert(!tokens.error && "No errors must have happend during tokenization to print the tokens!\n");

  printf("Printing tokens:\n");
  for (size_t i = 0; i < tokens.count; ++i) {
    printf("Type=%s", tokenTypeNames[tokens.buff[i].type]);
    if (tokens.buff[i].type == TOKENTYPE_NUMBER)
      printf(", Literal=%d", tokens.buff[i].literal);
    else if (tokens.buff[i].type == TOKENTYPE_OPERATOR)
      printf(", Operation=%s", operationNames[tokens.buff[i].operation]);
    printf("\n");
  }
}

#endif // _LEXER_H_
