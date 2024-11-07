#include "tokenizer.h"
#include "lexer.h"
#include "parser.h"

static const char* validateInput(int argc, char** argv);
static void printUsage(const char* program);
static bool handle(const char* input);


int main(int argc, char** argv)
{
  const char* input = validateInput(argc, argv);
  
  if (!input)
    return 1;

  bool result = handle(input);
  
  return result ? 0 : 1;
}


static const char* validateInput(int argc, char** argv)
{
  const char* program = argv[0];
  
  if (argc != 2)
  {
    printUsage(program);
    return NULL;
  }
  
  return argv[1];
}


static void printUsage(const char* program)
{
  fprintf(stderr, "ERROR: Invalid usage! Expected 1 argument.\n");
  fprintf(stderr, "USAGE: \"%s EXPRESSION\"\n", program);
  fprintf(stderr, "Example: %s \"10.5 + 30 - (2 * 5.2) / 7\"\n", program);
}


static bool handle(const char* input)
{
  // Tokenize input
  token_list_t tokens = {0};
  tokenize_input(&tokens, input);
  print_tokens(&tokens);
  
  // Lexing of the tokens
  lexer_t lexer = {0};
  lex_tokens(&lexer, &tokens);

  token_list_free(tokens);
  
  if (lexer.isError) {
    lexer_free(lexer);
    return false;
  }

  print_lexed_tokens(&lexer);

  // Parsing of the lexed tokens
  parse(&lexer);

  lexer_free(lexer);
  return true;
}
