#include "lexer.h"
#include "parser.h"

static void printUsage(const char* program)
{
  fprintf(stderr, "ERROR: Invalid usage! Expected 1 argument.\n");
  fprintf(stderr, "USAGE: \"%s EXPRESSION\"\n", program);
  fprintf(stderr, "Example: %s \"10.5 + 30 - (2 * 5.2) / 7\"\n", program);
}


int main(int argc, char** argv)
{
  const char* program = argv[0];

  if (argc != 2)
  {
    printUsage(program);
    return 1;
  }

  const char* input = argv[1];

  lexer_t lexer = {0};
  
  lexer_tokenize(&lexer, input, false);

  if (lexer.isError)
    return 1;

  lexer_print(&lexer);
  
  parse(&lexer);

  lexer_free(lexer);
  return 0;
}
