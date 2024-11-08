#include "tokenizer.h"
#include "lexer.h"
#include "parser.h"

static const char* validate_cli_input(int argc, char** argv);
static void print_usage(const char* program);
static bool handle_input(const char* input);


int main(int argc, char** argv)
{
  // TODO: Implement full CLI interface mode if the user does not run the program with a math expression directly.
  // This could then be used to support variables and solving for variables and store full math expressions with variables in memory for later
  // solving. Could be used to store a few base expressions into variables and then using them to calulate more complex expressions.

  const char* input = validate_cli_input(argc, argv);
  
  if (!input)
    return 1;

  bool result = handle_input(input);
  
  return result ? 0 : 1;
}


static const char* validate_cli_input(int argc, char** argv)
{
  const char* program = argv[0];
  
  if (argc != 2)
  {
    print_usage(program);
    return NULL;
  }
  
  return argv[1];
}


static void print_usage(const char* program)
{
  fprintf(stderr, "ERROR: Invalid usage! Expected 1 argument.\n");
  fprintf(stderr, "USAGE: \"%s EXPRESSION\"\n", program);
  fprintf(stderr, "Example: %s \"10.5 + 30 - (2 * 5.2) / 7\"\n", program);
}


static bool handle_input(const char* input)
{
  // Tokenize input
  token_list_t tokens = {0};
  tokenize_input(&tokens, input);
  print_tokens(&tokens);
  
  // Lexing of the tokens
  lexer_t lexer = {0};
  lex_tokens(&lexer, &tokens);

  token_list_free(tokens);
  
  // Check for occurred errors to prematurely stop execution.
  if (lexer.isError) {
    lexer_free(lexer);
    return false;
  }

  print_lexed_tokens(&lexer);

  // Parsing of the lexed tokens
  parse_lexer(&lexer);

  lexer_free(lexer);
  return true;
}
