#include "lexer.h"
#include "parser.h"

int main(int argc, char** argv)
{
  (void) argc;
  (void) argv;

  // TODO: Implement basic cli!
  
  const char* value = "  100.53 +  (4 *   6.4) /   8";

  token_list_t tokens = tokenize(value, strlen(value));
  print_tokens(tokens);

  parse(tokens);

  token_list_free(tokens);
  return 0;
}
