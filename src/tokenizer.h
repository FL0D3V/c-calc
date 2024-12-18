#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#define ARENA_IMPLEMENTATION
#include "arena.h"
#include "stringslice.h"
#include "global.h"
#include "helpers.h"


// Error handling
#define T_ERROR_NAME "TOKENIZATION-ERROR"
#define T_ERROR_NO_INPUT_GIVEN()          fprintf(stderr, T_ERROR_NAME ": No input given!\n")
#define T_ERROR_GIVEN_TOKENIZER_INVALID() fprintf(stderr, T_ERROR_NAME ": Can't print the tokenizer because an error happend!\n")

// For printing
#define IN_TOK_FMT "%.*s"
#define IN_TOK_ARG(tok) (int)(tok)->length, (tok)->value


typedef struct {
  const char* value;
  size_t length;
  size_t cursor;
} input_token_t;

typedef struct {
  input_token_t* items;
  size_t capacity;
  size_t count;
  bool isError;
} tokenizer_t;


#define tokenizer_append(a, tokenizer, val, len, curr) arena_da_append((a), (tokenizer), ((input_token_t) { .value = (val), .length = (len), .cursor = (curr) }))


// Tokenizes a collection of characters into a symbol which are not spaces and literal characters.
static bool next_symbol(arena_t* arena, string_slice_t* ss, tokenizer_t* tokenizer)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(ss);
  ASSERT_NULL(tokenizer);

  if (!ss_in_range(ss))
    return true;

  const char* startPtr = ss_get_current_ptr(ss);
  char current;
  size_t length = 0;

  do
  {
    current = ss_get_current(ss);

    if (isspace(current) || c_is_literal(current))
      break;

    ++length;
    ss_seek(ss);
  } while (ss_in_range(ss));

  if (length > 0)
    tokenizer_append(arena, tokenizer, startPtr, length, ss_current_pos(ss) - length);

  return length > 0;
}


// Tokenizes a single character literal if valid.
static bool next_literal(arena_t* arena, string_slice_t* ss, tokenizer_t* tokenizer)
{
  ASSERT_NULL(arena);
  ASSERT_NULL(ss);
  ASSERT_NULL(tokenizer);

  if (!ss_in_range(ss))
    return true;

  const char* currentPtr = ss_get_current_ptr(ss);
  bool isLiteral = c_is_literal(*currentPtr);

  if (isLiteral)
    tokenizer_append(arena, tokenizer, currentPtr, 1, ss_current_pos(ss));
  
  ss_seek(ss);
  
  return isLiteral;
}


tokenizer_t tokenizer_execute(arena_t* arena, const char* input)
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
    
    if (next_symbol(arena, &ss, &tokenizer)) continue;
    else if (next_literal(arena, &ss, &tokenizer)) continue;
    else
    {
      UNREACHABLE_MSG("Unhandled character!");
      tokenizer.isError = true;
      break;
    }
  }

  return tokenizer;
}


void tokenizer_print(const tokenizer_t* tokenizer)
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

#endif // _TOKENIZER_H_
