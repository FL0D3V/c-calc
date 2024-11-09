#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include "global.h"
#include "tokenizer.h"
#include "lexer.h"
#include "parser.h"

// The current version of the program.
#define CURRENT_VERSION version(0, 1, 0)


// This is usefull for: "PF_VERBOSE | PF_HELP"
typedef enum {
  PFF_ERROR      = 0,
  PFF_EXPRESSION = (1u << 0),
  PFF_VERBOSE    = (1u << 1),
  PFF_HELP       = (1u << 2),
  PFF_VERSION    = (1u << 3),
  // OTHER       = (1u << 4...'int' size in bit),
} e_program_function_flags;

typedef enum {
  PFT_VERBOSE,
  PFT_HELP,
  PFT_VERSION,

  PFT_COUNT,
  PFT_EXPRESSION,
  PFT_INVALID,
} e_program_function_type;

static_assert(PFT_COUNT == 3, "Amount of program-function-types have changed");

#define SHORT_PREFIX "-"
#define SHORT_PREFIX_LEN strlen(SHORT_PREFIX)
static const char* shortProgFuncTypeIdentifier[PFT_COUNT] = {
  [PFT_VERBOSE] = "vv",
  [PFT_HELP]    = "h",
  [PFT_VERSION] = "v",
};

#define LONG_PREFIX "--"
#define LONG_PREFIX_LEN strlen(LONG_PREFIX)
static const char* longProgFuncTypeIdentifier[PFT_COUNT] = {
  [PFT_VERBOSE] = "verbose",
  [PFT_HELP]    = "help",
  [PFT_VERSION] = "version",
};

#define IDENTIFIER_STRING_ARGS "%s%s"
#define _IDENTIFIER_STRING_ARGS_COL(col) "%s%" col "s"
#define IDENTIFIER_STRING_ARGS_COL_S _IDENTIFIER_STRING_ARGS_COL("-4")
#define IDENTIFIER_STRING_ARGS_COL_L _IDENTIFIER_STRING_ARGS_COL("-10")
#define short_full_identifier(index) SHORT_PREFIX, shortProgFuncTypeIdentifier[(index)]
#define long_full_identifier(index)  LONG_PREFIX, longProgFuncTypeIdentifier[(index)]

static const char* progFuncTypeDescriptions[PFT_COUNT] = {
  [PFT_VERBOSE] = "Executes a given math expression verbose. This means all steps from tokenization, lexing and parsing will get printed.",
  [PFT_HELP]    = "Shows the help menu.",
  [PFT_VERSION] = "Shows the current program version."
};

static e_program_function_type cstr_to_program_function_type(const char* cstr)
{
  if (!cstr)
    return PFT_INVALID;

  // No argument can have spaces so it must be an expression.
  if (strstr(cstr, " "))
    return PFT_EXPRESSION;

  const char* longPrefixStart = strstr(cstr, LONG_PREFIX);
  const char* shortPrefixStart = strstr(cstr, SHORT_PREFIX);

  const bool validLongPrefix = longPrefixStart && PTR_DIFF(cstr, longPrefixStart) == 0;
  const bool validShortPrefix = shortPrefixStart && PTR_DIFF(cstr, shortPrefixStart) == 0;

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
    case PFT_VERSION:    return PFF_VERSION;
    case PFT_INVALID:    return PFF_ERROR;
    case PFT_COUNT:
    default:
      UNREACHABLE("function_type_to_flag: Type not implemented!");
  }
}


// Program definition.
typedef struct {
  e_program_function_flags funcFlags;
  int argc;
  char* programName;
  char** argv;
  char* inputExpression;
} program_t;

static void program_init(program_t* prog, int argc, char** argv)
{
  prog->funcFlags = PFF_ERROR;

  // Store only the current program name without path: './bin/ccalc' -> 'ccalc'
  cstr_chop_till_last_delim(argv, '/');
  prog->programName = *argv;
  
  prog->argc = --argc;
  prog->argv = ++argv;
  prog->inputExpression = NULL;
}

static void program_set_error(program_t* prog)
{
  prog->funcFlags = PFF_ERROR;
  prog->inputExpression = NULL;
}



// Interface definitions.
program_t validate_cli_input(int argc, char** argv);
int handle_program(program_t* program);

// All program definitions.
static void print_usage(e_program_function_flags flags, const char* programName, int argc, char** argv);
static void print_help(const char* programName);
static void print_current_version();
static int handle_math_input(const char* input, bool verbose);



program_t validate_cli_input(int argc, char** argv)
{
  program_t prog = {0};

  program_init(&prog, argc, argv);

  // Do the pointer aritmethic on an independent variable, so the program keeps the valid state.
  char** currentArgv = prog.argv;

  // Invalid usage.
  if (prog.argc == 0)
  {
    program_set_error(&prog);
    return prog;
  }
  
  for (int i = 0; i < prog.argc; ++i)
  {
    const e_program_function_type func = cstr_to_program_function_type(*currentArgv);

    // Invalid usage.
    if (func == PFT_INVALID)
    {
      program_set_error(&prog);
      break;
    }

    const e_program_function_flags flag = function_type_to_flag(func);

    // Bit can't be set twice.
    if (is_bit_set(prog.funcFlags, flag))
    {
      program_set_error(&prog);
      break;
    }

    set_bit(prog.funcFlags, flag);

    // Set the input of the program when an expression was found.
    if (flag == PFF_EXPRESSION)
      prog.inputExpression = *currentArgv;

    currentArgv++;
  }

  return prog;
}


int handle_program(program_t* program)
{
  // Checking for invalid usage.
  if (program->funcFlags == PFF_ERROR ||
      is_only_bit_set(program->funcFlags, PFF_VERBOSE) ||
      is_not_only_bit_set(program->funcFlags, PFF_HELP) ||
      is_not_only_bit_set(program->funcFlags, PFF_VERSION))
  {
    print_usage(program->funcFlags, program->programName, program->argc, program->argv);
    return EXIT_FAILURE;
  }

  // Checking if the help function was called.
  if (is_only_bit_set(program->funcFlags, PFF_HELP))
  {
    print_help(program->programName);
    return EXIT_SUCCESS;
  }

  // Checking if the version function was called.
  if (is_only_bit_set(program->funcFlags, PFF_VERSION))
  {
    print_current_version();
    return EXIT_SUCCESS;
  }

  // Checking if an expression should get executed and if it should be verbose.
  if (is_bit_set(program->funcFlags, PFF_EXPRESSION))
    return handle_math_input(program->inputExpression, is_bit_set(program->funcFlags, PFF_VERBOSE));

  UNREACHABLE("handle_program: Flag not implemented!");
}



// All programs functions.

static void print_usage(e_program_function_flags flags, const char* programName, int argc, char** argv)
{
  ASSERT_NULL(programName);
  
  // PROG_NAME: invalid option -- 'ALL GIVEN PARAMS'
  // Try 'PROG_NAME --help' or '-h' for more information.

  fprintf(stderr, "%s: invalid option -- '", programName);
  for (int i = 0; i < argc; ++i)
  {
    if (i >= argc - 1) fprintf(stderr, "%s", *argv++);
    else fprintf(stderr, "%s ", *argv++);
  }
  fprintf(stderr, "'\n");

  // Print additional usage informations for specific arguments.
  if (is_only_bit_set(flags, PFF_VERBOSE))
  {
    fprintf(stderr, "Verbose execution:\n");
    fprintf(stderr, "  Usage: '" IDENTIFIER_STRING_ARGS " EXPRESSION' or '" IDENTIFIER_STRING_ARGS " EXPRESSION'\n", short_full_identifier(PFT_VERBOSE), long_full_identifier(PFT_VERBOSE));
  }

  fprintf(stderr, "'%s " IDENTIFIER_STRING_ARGS "' or '" IDENTIFIER_STRING_ARGS "' for more information.\n", programName, long_full_identifier(PFT_HELP), short_full_identifier(PFT_HELP));
}


static void print_help(const char* programName)
{
  ASSERT_NULL(programName);

  printf("Usage: %s [OPTION]... [EXPRESSION]\n", programName);
  printf("Execute simple to more complex math expressions in the terminal.\n");
  printf("This calculator supports decimals, multiple operators, simple math functions\n");
  printf("like 'sqrt' or 'sin', brackets ('(', ')') and multiple math constants like 'Pi'.\n");
  // Add more description here.
  
  printf("\n");
  
  printf("Example usage:\n");
  printf("  %s \"10.5 + 30 - sqrt(PI * 5.2) / 8\"\n", programName);
  
  printf("\n");
  
  printf("The options below may be used for printing the expression execution verbose\n");
  printf("or for example getting the current version.\n");

  for (size_t i = 0; i < PFT_COUNT; ++i)
    printf("  " IDENTIFIER_STRING_ARGS_COL_S " " IDENTIFIER_STRING_ARGS_COL_L "  %s\n", short_full_identifier(i), long_full_identifier(i), progFuncTypeDescriptions[i]);
  
  printf("\n");
  
  printf("Full documentation <https://github.com/FL0D3V/c-calc>\n");
}


static void print_current_version()
{
  printf("Current program version: " VERSION_FORMAT "\n", VERSION_ARGS(CURRENT_VERSION));
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

#endif // _PROGRAM_H_