#include "tokenizer.h"

int main(int argc, char** argv)
{
  (void) argc;
  (void) argv;

  const char* value = "  100.53 +  (4 *   6.4) /   8";

  token_list_t tokens = tokenize_ex(value, strlen(value), true);
  print_tokens(tokens);

  da_free(tokens);
  return 0;
}
