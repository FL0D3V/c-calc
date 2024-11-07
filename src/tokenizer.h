#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include "global.h"
#include "darray.h"
#include "stringslice.h"


typedef struct {
  char* items;
  size_t capacity;
  size_t count;

  // TODO: Rethink! Could be used to store the current cursor position of the original input string for this token.
  // To know exacly where an input error could have happend in the original input string, you could use the cursor plus the offset
  // in the current token string.
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
  //ss_seek_spaces(ss);

  if (!ss_in_range(ss))
    return false;

  char current = ss_get_current(ss);
  while (ss_in_range(ss) && !isspace(current) && !c_is_operator(current) && !c_is_bracket(current))
  {
    sb_append_char(tokenBuff, current);
    ss_seek(ss);
    if (ss_in_range(ss))
      current = ss_get_current(ss);
  }

  if (tokenBuff->count > 0)
    return true;

  return false;
}


void tokenize_input(token_list_t* tokens, const char* input)
{
  ASSERT_NULL(tokens);

  string_slice_t ss = {0};
  string_builder_t inputBuff = {0};
  //bool isDirty = false;
  
  ss_init(&ss, input);
  
  while (ss_in_range(&ss))
  {
    ss_seek_spaces(&ss);
    
    if (!ss_in_range(&ss))
      break;

    /*if (c_is_literal(current))
    {
      if (isDirty)
      {
        token_list_append(tokens, inputBuff.items, currentPos - inputBuff.count);
        sb_free(inputBuff);
        isDirty = false;
      }

      sb_append_char(&inputBuff, current);

      // Checks for completion of the current literal.
      if ((ss_can_peek(&ss) && !c_is_literal(ss_peek(&ss))) || !ss_can_peek(&ss))
      {
        token_list_append(tokens, inputBuff.items, currentPos - inputBuff.count);
        sb_free(inputBuff);
      }
    }*/
    if (next_cstr_token(&ss, &inputBuff))
    {
      size_t currentPos = ss_current_pos(&ss);

      token_list_append(tokens, inputBuff.items, currentPos - inputBuff.count);
      sb_free(inputBuff);

      continue;
    }
    else
    {
      // Because 'next_cstr_token' can seek to the end.
      if (!ss_in_range(&ss))
        break;
    }
    
    if (c_is_operator(ss_get_current(&ss)))
    {
      const char current = ss_get_current(&ss);
      size_t currentPos = ss_current_pos(&ss);

      /*if (isDirty)
      {
        token_list_append(tokens, inputBuff.items, currentPos - inputBuff.count);
        sb_free(inputBuff);
        isDirty = false;
      }*/

      sb_append_char(&inputBuff, current);
      token_list_append(tokens, inputBuff.items, currentPos - inputBuff.count);
      sb_free(inputBuff);
    }
    else if (c_is_bracket(ss_get_current(&ss)))
    {
      const char current = ss_get_current(&ss);
      size_t currentPos = ss_current_pos(&ss);

      /*if (isDirty)
      {
        token_list_append(tokens, inputBuff.items, currentPos - inputBuff.count);
        sb_free(inputBuff);
        isDirty = false;
      }*/

      sb_append_char(&inputBuff, current);
      token_list_append(tokens, inputBuff.items, currentPos - inputBuff.count);
      sb_free(inputBuff);
    }
    else
    {
      fprintf(stderr, "UNREACHABLE\n");
      goto defer;

      // For multi character function parsing
      //if (!isDirty)
      //  isDirty = true;
      //sb_append_char(&inputBuff, current);
    }

    ss_seek(&ss);
  }

defer:
  sb_free(inputBuff);
}


void print_tokens(token_list_t* tokens)
{
  ASSERT_NULL(tokens);

  printf("Printing tokenenized input ('%zu' tokens found):\n", tokens->count);

  for (size_t i = 0; i < tokens->count; i++) {
    input_token_t* currentToken = &tokens->items[i];
    printf("%zu: TOKEN('%s'); CURSOR(%zu)\n", i + 1, currentToken->items, currentToken->cursor);
  }
}

#endif // _TOKENIZER_H_
