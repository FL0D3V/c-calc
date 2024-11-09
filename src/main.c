#include "global.h"
#include "tokenizer.h"
#include "lexer.h"
#include "parser.h"


// Bit-Operations
#define set_bit(flags, bit) flags |= bit
#define unset_bit(flags, bit) flags &= ~bit
#define is_bit_set(flags, bit) (flags & bit)

#define is_only_bit_set(flags, bit) (flags == bit)


// This is usefull for: "PF_VERBOSE | PF_HELP"
typedef enum {
  PFF_ERROR      = 0,
  PFF_EXPRESSION = (1u << 0),
  PFF_VERBOSE    = (1u << 1),
  PFF_HELP       = (1u << 2),
  // OTHER       = (1u << 3...16),
} e_program_function_flags;

typedef enum {
  PFT_VERBOSE,
  PFT_HELP,

  PFT_COUNT,
  PFT_EXPRESSION,
  PFT_INVALID,
} e_program_function_type;

static_assert(PFT_COUNT == 2, "Amount of program-function-types have changed");

#define SHORT_PREFIX "-"
#define SHORT_PREFIX_LEN strlen(SHORT_PREFIX)
static const char* shortProgFuncTypeIdentifier[PFT_COUNT] = {
  [PFT_VERBOSE] = "v",
  [PFT_HELP]    = "h",
};

#define LONG_PREFIX "--"
#define LONG_PREFIX_LEN strlen(LONG_PREFIX)
static const char* longProgFuncTypeIdentifier[PFT_COUNT] = {
  [PFT_VERBOSE] = "verbose",
  [PFT_HELP]    = "help",
};

static const char* progFuncTypeDescriptions[PFT_COUNT] = {
  [PFT_VERBOSE] = "Executes a given math expression verbose. This means all steps from tokenization, lexing and parsing will get printed.",
  [PFT_HELP]    = "Shows the help menu.",
};

static e_program_function_type cstr_to_program_function_type(const char* cstr)
{
  if (!cstr)
    return PFT_INVALID;

  const char* spaceStart = strstr(cstr, " ");

  // No argument can have spaces so it must be an expression.
  if (spaceStart)
    return PFT_EXPRESSION;

  const char* longPrefixStart = strstr(cstr, LONG_PREFIX);
  const char* shortPrefixStart = strstr(cstr, SHORT_PREFIX);
  size_t ptrDiffLong = longPrefixStart ? PTR_DIFF(cstr, longPrefixStart) : 1000;
  size_t ptrDiffShort = shortPrefixStart ? PTR_DIFF(cstr, shortPrefixStart) : 1000;
  bool validLongPrefix = longPrefixStart && ptrDiffLong == 0;
  bool validShortPrefix = shortPrefixStart && ptrDiffShort == 0;

  // TODO: Rethink!

  for (size_t i = 0; i < PFT_COUNT; ++i)
  {
    if ((validLongPrefix && strcmp(cstr + LONG_PREFIX_LEN, longProgFuncTypeIdentifier[i]) == 0) ||
        (validShortPrefix && strcmp(cstr + SHORT_PREFIX_LEN, shortProgFuncTypeIdentifier[i]) == 0))
      return (e_program_function_type) i;
  }

  return PFT_EXPRESSION;
}

static e_program_function_flags function_type_to_flag(e_program_function_type type)
{
  switch (type) {
    case PFT_EXPRESSION: return PFF_EXPRESSION;
    case PFT_VERBOSE:    return PFF_VERBOSE;
    case PFT_HELP:       return PFF_HELP;
    case PFT_INVALID:    return PFF_ERROR;
    case PFT_COUNT:
    default:
      UNREACHABLE("Not implemented!");
  }
}

typedef struct {
  e_program_function_flags funcFlags;
  char* programName;
  char* input;
} program_t;



static program_t validate_cli_input(int argc, char** argv);
static int handle_program(program_t* program);
static void print_usage(const char* programName);
static void print_help();
static int handle_math_input(const char* input, bool verbose);


int main(int argc, char** argv)
{
  // TODO: Implement full CLI interface mode if the user does not run the program with a math expression directly.
  // This could then be used to support variables and solving for variables and store full math expressions with variables in memory for later
  // solving. Could be used to store a few base expressions into variables and then using them to calulate more complex expressions.

  program_t prog = validate_cli_input(argc, argv);

  return handle_program(&prog);
}



static program_t validate_cli_input(int argc, char** argv)
{
  program_t prog = {0};

  prog.programName = *argv++;
  
  e_program_function_flags flags = PFF_ERROR;
  char* input = NULL;

  // Invalid usage.
  if (argc == 1)
    return_defer();
  
  for (int i = 0; i < argc - 1; ++i)
  {
    const e_program_function_type func = cstr_to_program_function_type(*argv);

    // Invalid usage.
    if (func == PFT_INVALID)
      return_defer();

    const e_program_function_flags flag = function_type_to_flag(func);

    // Bit can't be set twice.
    if (is_bit_set(flags, flag))
    {
      flags = PFF_ERROR;
      input = NULL;
      return_defer();
    }

    set_bit(flags, flag);

    // Set the input of the program when an expression was found.
    if (flag == PFF_EXPRESSION)
      input = *argv;

    argv++;
  }

defer:
  prog.funcFlags = flags;
  prog.input = input;

  return prog;
}


static int handle_program(program_t* program)
{
  if (program->funcFlags == PFF_ERROR)
  {
    print_usage(program->programName);
    return EXIT_FAILURE;
  }

  if (is_only_bit_set(program->funcFlags, PFF_VERBOSE))
  {
    print_usage(program->programName);
    return EXIT_FAILURE;
  }

  if (is_only_bit_set(program->funcFlags, PFF_HELP))
  {
    print_help();
    return EXIT_SUCCESS;
  }

  if (is_bit_set(program->funcFlags, PFF_HELP) && is_bit_set(program->funcFlags, PFF_EXPRESSION))
  {
    print_usage(program->programName);
    return EXIT_FAILURE;
  }

  if (is_only_bit_set(program->funcFlags, PFF_EXPRESSION))
    return handle_math_input(program->input, false);

  if (is_bit_set(program->funcFlags, PFF_EXPRESSION) && is_bit_set(program->funcFlags, PFF_VERBOSE))
    return handle_math_input(program->input, true);

  UNREACHABLE("Not implemented!");
}


static void print_usage(const char* programName)
{
  ASSERT_NULL(programName);

  fprintf(stderr, "ERROR: Invalid arguments!\n");
  fprintf(stderr, "USAGE EXAMPLE:\n");
  fprintf(stderr, "\t%s \"10.5 + 30 - (2 * 5.2) / 7\"\n", programName);
  fprintf(stderr, "HELP MENU:\n");
  fprintf(stderr, "\t'%s' or '%s'\n", longProgFuncTypeIdentifier[PFT_HELP], shortProgFuncTypeIdentifier[PFT_HELP]);
}


static void print_help()
{
  printf("Help menu:\n");
  
  for (size_t i = 0; i < PFT_COUNT; ++i)
  {
    const char* longArg = longProgFuncTypeIdentifier[i];
    const char* shortArg = shortProgFuncTypeIdentifier[i];
    const char* argDescription = progFuncTypeDescriptions[i];

    printf("%s%s, %s%s: %s\n", LONG_PREFIX, longArg, SHORT_PREFIX, shortArg, argDescription);
  }
}


static int handle_math_input(const char* input, bool verbose)
{
  if (verbose)
    printf("Executing VERBOSE:\n");

  // Tokenize input
  token_list_t tokens = {0};
  tokenize_input(&tokens, input);
  
  if (verbose)
    print_tokens(&tokens);
  
  // Lexing of the tokens
  lexer_t lexer = {0};
  lex_tokens(&lexer, &tokens);
  token_list_free(tokens);

  // Check for occurred errors to prematurely stop execution.
  if (lexer.isError) {
    lexer_free(lexer);
    return EXIT_FAILURE;
  }

  if (verbose)
    print_lexed_tokens(&lexer);

  // Parsing of the lexed tokens
  // TODO: Implement!
  //parse_lexer(&lexer);

  lexer_free(lexer);
  return EXIT_SUCCESS;
}
