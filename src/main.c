#include "lexer.h"
#include "parser.h"

int main(int argc, char** argv)
{
  const char* program = argv[0];

  if (argc != 2)
  {
    fprintf(stderr, "ERROR: Invalid usage! Expected 1 argument.\n");
    fprintf(stderr, "USAGE: \"%s EXPRESSION\"\n", program);
    fprintf(stderr, "Example: %s \"10.5 + 30 - (2 * 5.2) / 7\"\n", program);
    return 1;
  }

  const char* input = argv[1];

  token_list_t tokens = {0};
  
  tokenize(&tokens, input, false);

  print_tokens(&tokens);
  
  parse(&tokens);

  da_free(tokens);
  return 0;
}
