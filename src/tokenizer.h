#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include "global.h"
#include "darray.h"
#include "stringslice.h"


#define t_return_defer()              goto defer
#define t_unreachable_defer(message)  do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); t_return_defer(); } while(0)


typedef struct {
  char* items;
  size_t capacity;
  size_t count;
  
  size_t cursor;
} input_token_t;

typedef struct {
  input_token_t* items;
  size_t capacity;
  size_t count;
} token_list_t;


#define token_list_append(tl, cstr, cur)  \
    do {                                  \
      input_token_t it = {0};             \
      it.cursor = (cur);                  \
      sb_append_cstr(&it, cstr);          \
      da_append(tl, it);                  \
    } while(0)

#define token_list_free(tl) sl_free(tl)


static bool next_cstr_token(string_slice_t* ss, string_builder_t* tokenBuff)
{
  sb_free(*tokenBuff);

  if (!ss_in_range(ss))
    return false;

  char current = ss_get_current(ss);

  // TODO: Implement the usage of negative numbers like e.g. '-5 * 10'.
  //       This means that the '-' in front of '5' should NOT be an operator.

  do
  {
    if (isspace(current) ||
        c_is_operator(current) ||
        c_is_paren(current))
      break;

    sb_append_char(tokenBuff, current);
    ss_seek(ss);

    if (ss_in_range(ss))
      current = ss_get_current(ss);
  } while (ss_in_range(ss));

  /* OLD:
  while (
      ss_in_range(ss) &&
      !isspace(current) &&
      !c_is_operator(current) &&
      !c_is_paren(current))
  {
    sb_append_char(tokenBuff, current);
    ss_seek(ss);

    if (ss_in_range(ss))
      current = ss_get_current(ss);
  }
  */

  if (tokenBuff->count > 0)
    return true;

  return false;
}


void tokenize_input(token_list_t* tokens, const char* input)
{
  ASSERT_NULL(tokens);

  string_slice_t ss = {0};
  string_builder_t inputBuff = {0};
  
  ss_init(&ss, input);
  
  while (ss_in_range(&ss))
  {
    ss_seek_spaces(&ss);
    
    // Because 'ss_seek_spaces' can seek to the end.
    if (!ss_in_range(&ss))
      break;

    if (next_cstr_token(&ss, &inputBuff))
    {
      token_list_append(tokens, inputBuff.items, ss_current_pos(&ss) - inputBuff.count);
      sb_free(inputBuff);

      continue;
    }
    else
    {
      // Because 'next_cstr_token' can seek to the end.
      if (!ss_in_range(&ss))
        break;
    }

    const char current = ss_get_current(&ss);
    const size_t currentPos = ss_current_pos(&ss);

    if (c_is_operator(current))
    {
      sb_append_char(&inputBuff, current);
      token_list_append(tokens, inputBuff.items, currentPos - inputBuff.count);
      sb_free(inputBuff);
    }
    else if (c_is_paren(current))
    {
      sb_append_char(&inputBuff, current);
      token_list_append(tokens, inputBuff.items, currentPos - inputBuff.count);
      sb_free(inputBuff);
    }
    else t_unreachable_defer("Unhandled character!");

    ss_seek(&ss);
  }

defer:
  sb_free(inputBuff);
}


void print_tokens(token_list_t* tokens)
{
  ASSERT_NULL(tokens);

  printf("Printing tokenenized input ('%zu' tokens):\n", tokens->count);

  for (size_t i = 0; i < tokens->count; i++) {
    input_token_t* currentToken = &tokens->items[i];
    printf("Token('%s')\n", currentToken->items);
  }
}

#endif // _TOKENIZER_H_
