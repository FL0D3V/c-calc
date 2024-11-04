#include "lexer.h"

int main(int argc, char** argv)
{
  (void) argc;
  (void) argv;

  const char* value = "100 + (4 * 6) / 8";
  
  printf("\nInput='%s'; Length=%zu\n", value, strlen(value));

  tokenizer_out_t tokens = tokenize(value, strlen(value));

  if (tokens.error) {
    printf(tokens.error);
    return EXIT_FAILURE;
  }

  print_tokens(tokens);

  return EXIT_SUCCESS;
}
