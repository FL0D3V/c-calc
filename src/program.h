#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include "config.h"
#include "versioning.h"
#include "parser.h"


// Program informations
#define COPYRIGHT_CREATOR "Florian Maier"
#define COPYRIGHT_YEAR "2024"
#define PROGRAM_LICENCE_LINK "https://github.com/FL0D3V/c-calc/blob/main/LICENSE"
#define PROGRAM_LICENCE "MIT"
#define FULL_DOCUMENTATION_LINK "https://github.com/FL0D3V/c-calc"
#define WRITTEN_BY "Florian Maier"
#define CREATOR_LINK "https://github.com/FL0D3V"
#define CURRENT_PROGRAM_VERSION version(0, 0, 1)  // MAJOR, MINOR, REVISION/PATCH



// All enums
// This is usefull for: "PF_VERBOSE | PF_HELP"
typedef enum {
  PFF_ERROR      = 0,
  PFF_EXPRESSION = (1u << 0),
  PFF_VERBOSE    = (1u << 1),
  PFF_HELP       = (1u << 2),
  PFF_VERSION    = (1u << 3),
  PFF_TEST_AST   = (1u << 4),   // TODO: Remove later! This is just for testing.
} e_program_function_flags;


typedef enum {
  PFT_VERBOSE,
  PFT_HELP,
  PFT_VERSION,
  PFT_TEST_AST, // TODO: Remove later! This is just for testing.
  
  PFT_COUNT,
  PFT_EXPRESSION,
  PFT_INVALID,
} e_program_function_type;

static_assert(PFT_COUNT == 4, "Amount of program-function-types have changed");

#define SHORT_PREFIX "-"
#define SHORT_PREFIX_LEN strlen(SHORT_PREFIX)
const char* shortProgFuncTypeIdentifier[PFT_COUNT] = {
  [PFT_VERBOSE]  = "vv",
  [PFT_HELP]     = "h",
  [PFT_VERSION]  = "v",
  [PFT_TEST_AST] = "ta", // TODO: Remove later! This is just for testing.
};

#define LONG_PREFIX "--"
#define LONG_PREFIX_LEN strlen(LONG_PREFIX)
const char* longProgFuncTypeIdentifier[PFT_COUNT] = {
  [PFT_VERBOSE]  = "verbose",
  [PFT_HELP]     = "help",
  [PFT_VERSION]  = "version",
  [PFT_TEST_AST] = "test-ast", // TODO: Remove later! This is just for testing.
};

// TODO: Rethink:
// '-e' [EXPRESSION]
// For executing a given math expression.
// This could be usefull when implementing a full CLI and allowing for multiple inputs in the same session with
// variable storage and other features.

#define IDENTIFIER_STRING_ARGS "%s%s"
#define _IDENTIFIER_STRING_ARGS_COL(col) "%s%" col "s"
#define IDENTIFIER_STRING_ARGS_COL_S _IDENTIFIER_STRING_ARGS_COL("-5")
#define IDENTIFIER_STRING_ARGS_COL_L _IDENTIFIER_STRING_ARGS_COL("-15")
#define short_full_identifier(index) SHORT_PREFIX, shortProgFuncTypeIdentifier[(index)]
#define long_full_identifier(index)  LONG_PREFIX, longProgFuncTypeIdentifier[(index)]

const char* progFuncTypeDescriptions[PFT_COUNT] = {
  [PFT_VERBOSE]  = "Execute the given expression with verbose logging and exit.",
  [PFT_HELP]     = "Display this help and exit.",
  [PFT_VERSION]  = "Output version information and exit.",
  [PFT_TEST_AST] = "Tests the ast generation and evaluation of pre defined expressions.", // TODO: Remove later! This is just for testing.
};



// Type definitions
typedef struct {
  e_program_function_flags funcFlags;
  int argc;
  char* programName;
  char** argv;
  char* inputExpression;
} program_t;



static e_program_function_type cstr_to_program_function_type(const char* cstr)
{
  if (!cstr)
    return PFT_INVALID;

  // No argument can have spaces so it must be an expression.
  // TODO: Change to -e for an expression which needs to accept an argument, the expression itself.
  if (strstr(cstr, " "))
    return PFT_EXPRESSION;

  // Checks for a present prefix and returns the pointer to the location in the input.
  const char* longPrefixStart = strstr(cstr, LONG_PREFIX);
  const char* shortPrefixStart = strstr(cstr, SHORT_PREFIX);
  
  // Checks if a specific prefix was found and if it is placed in the beginning of the string. E.g. "--COMMAND"
  const bool validLongPrefix = longPrefixStart && ((size_t)longPrefixStart - (size_t)cstr) == 0;
  const bool validShortPrefix = shortPrefixStart && ((size_t)shortPrefixStart - (size_t)cstr) == 0;

  for (size_t i = 0; i < PFT_COUNT; ++i)
  {
    if ((validLongPrefix && strcmp(cstr + LONG_PREFIX_LEN, longProgFuncTypeIdentifier[i]) == 0) ||
        (validShortPrefix && strcmp(cstr + SHORT_PREFIX_LEN, shortProgFuncTypeIdentifier[i]) == 0))
      return (e_program_function_type) i;
  }

  // TODO: Remove! -> Maybe error here!
  return PFT_EXPRESSION;
}


static e_program_function_flags function_type_to_flag(e_program_function_type type)
{
  switch (type) {
    case PFT_EXPRESSION: return PFF_EXPRESSION;
    case PFT_VERBOSE:    return PFF_VERBOSE;
    case PFT_HELP:       return PFF_HELP;
    case PFT_VERSION:    return PFF_VERSION;
    case PFT_TEST_AST:   return PFF_TEST_AST; // TODO: Remove later! This is just for testing.
    case PFT_INVALID:    return PFF_ERROR;
    case PFT_COUNT:
    default:
      UNREACHABLE("function_type_to_flag: Type not implemented!");
  }
}


static inline void program_init(program_t* prog, int argc, char** argv)
{
  prog->funcFlags = PFF_ERROR;

  // Store only the current program name without path: './bin/PROGRAM_NAME' -> 'PROGRAM_NAME'
  cstr_chop_till_last_delim(argv, '/');
  prog->programName = *argv;
  
  prog->argc = --argc;
  prog->argv = ++argv;
  prog->inputExpression = NULL;
}


static inline void program_set_error(program_t* prog)
{
  prog->funcFlags = PFF_ERROR;
  prog->inputExpression = NULL;
}



// All program definitions.
static void print_usage(e_program_function_flags flags, const char* programName, int argc, char** argv);
static void print_help(const char* programName);
static void print_current_version(const char* programName);
static int handle_math_input(const char* input, bool verbose);
static void test_ast_eval();



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
      is_not_only_bit_set(program->funcFlags, PFF_VERSION) ||
      is_not_only_bit_set(program->funcFlags, PFF_TEST_AST)) // TODO: Remove later! Just for testing.
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
    print_current_version(program->programName);
    return EXIT_SUCCESS;
  }

  // TODO: Remove later! Just for testing.
  if (is_only_bit_set(program->funcFlags, PFF_TEST_AST))
  {
    test_ast_eval();
    return EXIT_SUCCESS;
  }

  // Checking if an expression should get executed and if it should be verbose.
  if (is_bit_set(program->funcFlags, PFF_EXPRESSION))
  {
    // TODO: Rethink! Changes to the simple expression eval mode with limited features.
    change_global_program_mode(GPM_SINGLE_CLI_EXPRESSION_ARG);

    return handle_math_input(program->inputExpression, is_bit_set(program->funcFlags, PFF_VERBOSE));
  }

  UNREACHABLE("handle_program: Flag not implemented!");
}



// All programs functions.
static int handle_math_input(const char* input, bool verbose)
{
  if (verbose) printf("Executing VERBOSE:\n");

  tokenizer_t tokenizer = tokenizer_execute(input);
  
  if (tokenizer.isError) {
    tokenizer_free(tokenizer);
    return EXIT_FAILURE;
  }

  if (verbose) tokenizer_print(&tokenizer);
  
  lexer_t lexer = lexer_execute(&tokenizer);
  tokenizer_free(tokenizer);

  if (lexer.isError) {
    lexer_free(lexer);
    return EXIT_FAILURE;
  }

  if (verbose) lexer_print(&lexer);
  
  // TODO: Change to global arena for all allocations.
  node_arena_t nodeArena = {0};
  node_t* rootNode = parser_execute(&nodeArena, &lexer);
  lexer_free(lexer);

  if (!rootNode)
    return EXIT_FAILURE;

  if (verbose) print_node(rootNode, true);

  node_t* evaluatedNode = eval(&nodeArena, rootNode);
  
  if (!evaluatedNode)
  {
    node_arena_free(&nodeArena);
    return EXIT_FAILURE;
  }

  printf("Result = " DOUBLE_PRINT_FORMAT "\n", evaluatedNode->as.constant);

  node_arena_free(&nodeArena);
  return EXIT_SUCCESS;
}


static void print_usage(e_program_function_flags flags, const char* programName, int argc, char** argv)
{
  ASSERT_NULL(programName);
  
  // PROG_NAME: invalid option -- 'ALL GIVEN PARAMS'
  // Try 'PROG_NAME --help' or '-h' for more information.

  fprintf(stderr, "%s: invalid option -- '", programName);
  for (int i = 0; i < argc; ++i)
  {
    if (i >= argc - 1) fprintf(stderr, "%s", *argv++);
    else fprintf(stderr, "%s", *argv++);
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

  // Prints the usage and an example.
  printf("Usage: %s [OPTION]... [EXPRESSION]\n", programName);
  printf("Execute simple to more complex math expressions in the terminal.\n");
  printf("\n");
  printf("Example usage:\n");
  printf("  %s \"10.5 + 30 - sqrt(PI * 5.2) / 8\"\n", programName);
  printf("\n");


  // Prints the cli options.
  printf("The options below may be used for printing the expression execution verbose\n");
  printf("or for example getting the current version.\n");
  for (size_t i = 0; i < PFT_COUNT; ++i)
    printf("  " IDENTIFIER_STRING_ARGS_COL_S " " IDENTIFIER_STRING_ARGS_COL_L "  %s\n", short_full_identifier(i), long_full_identifier(i), progFuncTypeDescriptions[i]);
  
  printf("\n");


  // Prints program inforamtions.
  printf("Supported operators [USAGE, NAME]:\n");
  for (size_t i = 0; i < OP_COUNT; ++i)
    printf("  %-10c%s\n", operatorTypeIdentifiers[i], operatorTypeNames[i]);
  printf("\n");

  printf("Supported functions [USAGE(x), DESCRIPTION]:\n");
  for (size_t i = 0; i < FT_COUNT; ++i)
    printf("  %-10s%s\n", functionTypeIdentifiers[i], functionTypeDescriptions[i]);
  
  printf("\n");

  printf("Supported math-constants [USAGE, NAME]:\n");
  for (size_t i = 0; i < MC_COUNT; ++i)
    printf("  %-10s%s\n", mathConstantTypeIdentifiers[i], mathConstantTypeNames[i]);
  
  printf("\n");

  print_global_program_config_help();

  printf("\n");
  
  printf("Full documentation <" FULL_DOCUMENTATION_LINK ">\n");
}


static void print_current_version(const char* programName)
{
  ASSERT_NULL(programName);

  printf("%s " VERSION_FORMAT "\n", programName, VERSION_ARGS(CURRENT_PROGRAM_VERSION));
  printf("Copyright (c) " COPYRIGHT_YEAR " " COPYRIGHT_CREATOR ".\n");
  printf("License " PROGRAM_LICENCE ": <" PROGRAM_LICENCE_LINK ">.\n");
  printf("This is free software: you are free to change and redistribute it.\n");
  printf("There is NO WARRANTY, to the extent permitted by law.\n");
  printf("\n");
  printf("Written by " WRITTEN_BY " <" CREATOR_LINK ">.\n");
}


// INFO: Just for testing! Remove later!
static void test_ast_eval()
{
  node_arena_t arena = {0};
  bool freeAfterEachTest = false;

  printf("AST testing:\n\n");

  // TEST 1
  printf("Test 1:\n");
  {
    // IN: "1 + 2 + (PI ^ 2) / 3"
    // AST: add(1.00000, add(2.00000, divide(paren(pow(3.14159, 2.00000)), 3.00000)))
    // = 6,28986
    node_t* test =
      node_binop(&arena, 0, NO_ADD,
        node_constant(&arena, 0, 1),
        node_binop(&arena, 0, NO_ADD,
          node_constant(&arena, 0, 2),
          node_binop(&arena, 0, NO_DIV,
            node_paren(&arena, 0,
              node_binop(&arena, 0, NO_POW,
                node_constant(&arena, 0, mathConstantTypeValues[MC_PI]),
                node_constant(&arena, 0, 2)
              )
            ),
            node_constant(&arena, 0, 3)
          )
        )
      );

    printf("Input = 1 + 2 + (PI ^ 2) / 3\n");
    print_node(test, true);
    
    node_t* evaluated = eval(&arena, test);
    if (evaluated)
      printf("= " DOUBLE_PRINT_FORMAT "\n", evaluated->as.constant);

    printf("\n");

    if (freeAfterEachTest) node_arena_free(&arena);
  }


  // TEST 2
  printf("Test 2:\n");
  {
    // IN: "ln(10)"
    // AST: ln(10)
    // = 2.30258
    node_t* test =
      node_func(&arena, 0, NF_LN,
        node_constant(&arena, 0, 10));
    
    printf("Input = ln(10)\n");
    print_node(test, true);
    
    node_t* evaluated = eval(&arena, test);
    if (evaluated)
      printf("= " DOUBLE_PRINT_FORMAT "\n", evaluated->as.constant);

    printf("\n");

    if (freeAfterEachTest) node_arena_free(&arena);
  }


  // TEST 3
  printf("Test 3:\n");
  {
    // IN: "100.53 + sqrt(3.5 - EN) + cos(44.23 * 6.4^2) / 8.3 + ln(10) - PI + ln(5^EC)"
    // AST: add(100.53000, add(sqrt(substract(3.50000, 2.71828)), add(divide(acos(multiply(44.23000, pow(6.40000, 2.00000))), 8.30000), substract(ln(10.00000), add(3.14159, ln(pow(5.00000, 0.57722)))))))
    // = 317.91853
    node_t* test =
      node_binop(&arena, 0, NO_ADD,
        node_constant(&arena, 0, 100.53),
        node_binop(&arena, 0, NO_ADD,
          node_func(&arena, 0, NF_SQRT,
            node_binop(&arena, 0, NO_SUB,
              node_constant(&arena, 0, 3.5),
              node_constant(&arena, 0, mathConstantTypeValues[MC_EULERS_NUMBER])
            )
          ),
          node_binop(&arena, 0, NO_ADD,
            node_binop(&arena, 0, NO_DIV,
              node_func(&arena, 0, NF_COS,
                node_binop(&arena, 0, NO_MUL,
                  node_constant(&arena, 0, 44.23),
                  node_binop(&arena, 0, NO_POW,
                    node_constant(&arena, 0, 6.4),
                    node_constant(&arena, 0, 2)
                  )
                )
              ),
              node_constant(&arena, 0, 8.3)
            ),
            node_binop(&arena, 0, NO_SUB,
              node_func(&arena, 0, NF_LN,
                node_constant(&arena, 0, 10)
              ),
              node_binop(&arena, 0, NO_ADD,
                node_constant(&arena, 0, mathConstantTypeValues[MC_PI]),
                node_func(&arena, 0, NF_LN,
                  node_binop(&arena, 0, NO_POW,
                    node_constant(&arena, 0, 5),
                    node_constant(&arena, 0, mathConstantTypeValues[MC_EULERS_CONSTANT])
                  )
                )
              )
            )
          )
        )
      );

    printf("Input = 100.53 + sqrt(3.5 - EN) + cos(44.23 * 6.4^2) / 8.3 + ln(10) - PI + ln(5^EC)\n");
    print_node(test, true);
    
    node_t* evaluated = eval(&arena, test);
    if (evaluated)
      printf("= " DOUBLE_PRINT_FORMAT "\n", evaluated->as.constant);

    printf("\n");

    if (freeAfterEachTest) node_arena_free(&arena);
  }


  // TEST 4
  printf("Test 4:\n");
  {
    // IN: "10.5 * exp(4)"
    // AST: mult(10.5, exp(4))
    // = 573.28058
    node_t* test =
      node_binop(&arena, 0, NO_MUL,
        node_constant(&arena, 0, 10.5),
        node_func(&arena, 0, NF_EXP,
          node_constant(&arena, 0, 4)
        )
      );
    
    printf("Input = 10.5 * exp(4)\n");
    print_node(test, true);
    
    node_t* evaluated = eval(&arena, test);
    if (evaluated)
      printf("= " DOUBLE_PRINT_FORMAT "\n", evaluated->as.constant);

    printf("\n");

    if (freeAfterEachTest) node_arena_free(&arena);
  }


  // TEST 5
  printf("Test 5:\n");
  {
    // IN: "10 + 5 / (5 * 0)"
    // AST: "add(10, divide(5, paren(mult(5, 0))))"
    // = ERR (Divide by Zero)
    node_t* test =
      node_binop(&arena, 3, NO_ADD,
        node_constant(&arena, 0, 10),
        node_binop(&arena, 6, NO_DIV,
          node_constant(&arena, 4, 5),
          node_paren(&arena, 8,
            node_binop(&arena, 10, NO_MUL,
              node_constant(&arena, 9, 5),
              node_constant(&arena, 11, 0)
            )
          )
        )
      );

    printf("Input = 10 + 5 / (5 * 0)\n");
    print_node(test, true);

    node_t* evaluated = eval(&arena, test);
    if (evaluated)
      printf("= " DOUBLE_PRINT_FORMAT "\n", evaluated->as.constant);

    printf("\n");

    if (freeAfterEachTest) node_arena_free(&arena);
  }

  
  // TEST 6
  printf("Test 6:\n");
  {
    // IN: "(5 * 0)"
    // AST: "paren(mult(5, 0))"
    // = 5
    node_t* test =
      node_paren(&arena, 0,
        node_binop(&arena, 0, NO_MUL,
          node_constant(&arena, 0, 5),
          node_constant(&arena, 0, 0)
        )
      );

    printf("Input = (5 * 0)\n");
    print_node(test, true);

    node_t* evaluated = eval(&arena, test);
    if (evaluated)
      printf("= " DOUBLE_PRINT_FORMAT "\n", evaluated->as.constant);

    printf("\n");

    if (freeAfterEachTest) node_arena_free(&arena);
  }


  // TEST 7
  printf("Test 7:\n");
  {
    // IN: "10 / 0"
    // AST: "div(10, 0)"
    // = ERR (Divide by Zero)
    node_t* test =
      node_binop(&arena, 2, NO_DIV,
        node_constant(&arena, 0, 10),
        node_constant(&arena, 4, 0)
      );

    printf("Input = 10 / 0\n");
    print_node(test, true);

    node_t* evaluated = eval(&arena, test);
    if (evaluated)
      printf("= " DOUBLE_PRINT_FORMAT "\n", evaluated->as.constant);

    printf("\n");

    if (freeAfterEachTest) node_arena_free(&arena);
  }


  // TEST 8
  printf("Test 8:\n");
  {
    // IN: "10 / (4)"
    // AST: "div(10, paren(4))"
    // = 2.5
    node_t* test =
      node_binop(&arena, 2, NO_DIV,
        node_constant(&arena, 0, 10),
        node_paren(&arena, 4,
          node_constant(&arena, 5, 4)
        )
      );

    printf("Input = 10 / (4)\n");
    print_node(test, true);

    node_t* evaluated = eval(&arena, test);
    if (evaluated)
      printf("= " DOUBLE_PRINT_FORMAT "\n", evaluated->as.constant);

    printf("\n");

    if (freeAfterEachTest) node_arena_free(&arena);
  }


  if (!freeAfterEachTest) node_arena_free(&arena);
}

#endif // _PROGRAM_H_
