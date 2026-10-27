#include <versioning.h>
#include <arena.h>
#define ARENA_IMPLEMENTATION
#define FLAG_IMPLEMENTATION
#define FLAG_PUSH_DASH_DASH_BACK
#include <flag.h>

#include "parser.h"

// Program informations
#define COPYRIGHT_CREATOR "Florian Maier"
#define COPYRIGHT_YEAR "2024-2026"
#define PROGRAM_LICENCE_LINK "https://github.com/FL0D3V/c-calc/blob/main/LICENSE"
#define PROGRAM_LICENCE "MIT"
#define FULL_DOCUMENTATION_LINK "https://github.com/FL0D3V/c-calc"
#define CREATOR_LINK "https://github.com/FL0D3V"
// MAJOR, MINOR, REVISION/PATCH, BUILD
#define CURRENT_PROGRAM_VERSION VERSION_NEW_EX(0, 0, 1, 1);

// TODO: Rethink!
#define COMPILER_VERSION "gcc version 14.2.0 (Debian 14.2.0-6)"
#define GIT_HASH "ca4cfbe4eb4217a42dcc832bbdd47e7849160764"
#define BUILD_TIME "2026-10-25 03:45:00"

typedef struct command command_t;

struct command
{
  const char *name;
  const char *short_description;
  const char *extra_description;
  const char *signature;
  bool deprecated;
  bool (*run)(command_t *self, const char *programName, int argc, char **argv);
};

static bool math_run(command_t *self, const char *programName, int argc, char **argv);
static bool help_run(command_t *self, const char *programName, int argc, char **argv);
static bool version_run(command_t *self, const char *programName, int argc, char **argv);
static bool test_run(command_t *self, const char *programName, int argc, char **argv);

command_t commands[] = {
    {
        .name = "input",
        .signature = "[OPTIONS] [EXPRESSION...]",
        .short_description = "Execute the given expression with verbose logging and exit.",
        .run = math_run,
    },
    {
        .name = "help",
        .signature = "[OPTIONS]",
        .short_description = "Display this help and exit.",
        .run = help_run,
    },
    {
        .name = "version",
        .signature = "[OPTIONS]",
        .short_description = "Output version information and exit.",
        .run = version_run,
    },
    {
        .name = "test",
        .signature = "[OPTIONS]",
        .short_description =
            "Tests the AST generation and evaluation of pre defined expressions.",
        .run = test_run,
    },
};

void print_command_usage(FILE *sink, command_t *command, const char *program_name, void *c)
{
  fprintf(sink, "Usage:\n");
  fprintf(sink, "    %s %s", program_name, command->name);
  if (command->signature)
  {
    fprintf(sink, " %s", command->signature);
  }
  fprintf(sink, "\n");
  fprintf(sink, "\n");

  if (command->short_description)
  {
    fprintf(sink, "Description:\n");
    fprintf(sink, "    %s", command->short_description);
    if (command->extra_description)
    {
      fprintf(sink, " %s", command->extra_description);
    }
    fprintf(sink, "\n");
    fprintf(sink, "\n");
  }

  if (c)
  {
    fprintf(sink, "OPTIONS:\n");
    flag_c_print_options(c, sink);
    fprintf(sink, "\n");
  }
}

void print_available_commands(FILE *sink, const char *programName, bool deprecated)
{
  const size_t arrLen = ARRAY_LEN(commands);

  int max_width = 0;
  for (size_t i = 0; i < arrLen; ++i)
  {
    int width = strlen(commands[i].name);
    if (width > max_width)
      max_width = width;
  }

  fprintf(sink, "Available commands:\n");
  for (size_t i = 0; i < arrLen; ++i)
  {
    const command_t *current = &commands[i];
    if (current->deprecated)
      continue;
    fprintf(sink, "    %-*s   %s\n", max_width, current->name, current->short_description);
  }
  fprintf(sink, "\n");

  if (deprecated)
  {
    int deprecatedCommands = 0;
    fprintf(sink, "Deprecated commands:\n");
    for (size_t i = 0; i < arrLen; ++i)
    {
      const command_t *current = &commands[i];
      if (!current->deprecated)
        continue;
      fprintf(sink, "    %-*s - %s\n", max_width, current->name, current->short_description);
      deprecatedCommands++;
    }
    if (deprecatedCommands == 0)
      fprintf(sink, "    No deprecated commands found.\n");
    fprintf(sink, "\n");
  }

  fprintf(sink, "Use `%s <COMMAND> -h` to learn more about a command.\n", programName);
}

static bool math_run(command_t *self, const char *programName, int argc, char **argv)
{
  bool verbose = false;
  bool help = false;

  void *c = flag_c_new(programName);
  flag_c_bool_var(c, &help, "h", false, "Print this help message");
  flag_c_bool_var(c, &verbose, "v", NULL, "Execute the given expression with verbose logging");

  if (!flag_c_parse(c, argc, argv))
  {
    print_command_usage(stderr, self, programName, c);
    flag_c_print_error(c, stderr);
    return false;
  }

  argc = flag_c_rest_argc(c);
  argv = flag_c_rest_argv(c);

  if (help)
  {
    print_command_usage(stdout, self, programName, c);
    return true;
  }

  if (argc <= 0)
  {
    print_command_usage(stderr, self, programName, c);
    fprintf(stderr, "ERROR: No math expression provided!\n");
    return false;
  }

  if (argc != 1)
  {
    print_command_usage(stderr, self, programName, c);
    fprintf(stderr, "ERROR: Too many arguments provided! Expected only a math expression.\n");
    return false;
  }

  flag_c_free(c);

  arena_t arena = {0};

  // TODO: Rethink!
  tokenizer_t tokenizer = tokenizer_execute(&arena, *argv);

  if (tokenizer.isError)
  {
    arena_free(&arena);
    return false;
  }

  if (verbose)
    tokenizer_print(&tokenizer);

  lexer_t lexer = lexer_execute(&arena, &tokenizer);

  if (lexer.isError)
  {
    arena_free(&arena);
    return false;
  }

  if (verbose)
    lexer_print(&lexer);

  node_t *rootNode = parser_execute(&arena, &lexer);

  if (!rootNode)
  {
    arena_free(&arena);
    return false;
  }

  if (verbose)
    print_node(rootNode, true);

  ast_eval_res_t result = ast_eval(&arena, rootNode);

  if (result.error)
  {
    arena_free(&arena);
    return false;
  }

  // Print result
  printf(">>> " DOUBLE_PRINT_FORMAT "\n", result.value);

  // Cleanup
  arena_free(&arena);
  return true;
}

static bool version_run(command_t *self, const char *programName, int argc, char **argv)
{
  bool help = false;

  void *c = flag_c_new(programName);
  flag_c_bool_var(c, &help, "h", false, "Print this help message");

  if (!flag_c_parse(c, argc, argv))
  {
    print_command_usage(stderr, self, programName, c);
    flag_c_print_error(c, stderr);
    return false;
  }

  argc = flag_c_rest_argc(c);
  argv = flag_c_rest_argv(c);

  if (help)
  {
    print_command_usage(stdout, self, programName, c);
    return true;
  }

  flag_c_free(c);

  const version_t v = CURRENT_PROGRAM_VERSION;
#ifdef GIT_HASH
  printf("%s " VERSION_FORMAT " commit %s\n", programName, VERSION_ARGS(v), GIT_HASH);
#else
  printf("%s " VERSION_FORMAT "\n", programName, VERSION_ARGS(v));
#endif // GIT_HASH
#ifdef BUILD_TIME
  printf("Build time: %s\n", BUILD_TIME);
#endif // BUILD_TIME
#ifdef COMPILER_VERSION
  printf("Compiler: %s\n", COMPILER_VERSION);
#endif // COMPILER_VERSION

  printf("\n");
  printf("Copyright (c): " COPYRIGHT_YEAR " " COPYRIGHT_CREATOR "\n");
  printf("License: " PROGRAM_LICENCE " <" PROGRAM_LICENCE_LINK ">\n");
  printf("More information: <" FULL_DOCUMENTATION_LINK ">\n");
  printf("\n");
  printf("This is FREE software, you are free to change and redistribute it.\n");
  printf("There is NO WARRANTY, to the extent permitted by law.\n");
  printf("\n");
  printf("More about me: <" CREATOR_LINK ">\n");

  return true;
}

static bool help_run(command_t *self, const char *programName, int argc, char **argv)
{
  bool help = false;
  bool deprecated = false;

  void *c = flag_c_new(programName);
  flag_c_bool_var(c, &help, "h", false, "Print this help message");
  flag_c_bool_var(c, &deprecated, "d", false, "Print deprecated commands");

  if (!flag_c_parse(c, argc, argv))
  {
    print_command_usage(stderr, self, programName, c);
    flag_c_print_error(c, stderr);
    return false;
  }

  argc = flag_c_rest_argc(c);
  argv = flag_c_rest_argv(c);

  if (help)
  {
    print_command_usage(stdout, self, programName, c);
    return true;
  }

  flag_c_free(c);

  if (argc <= 0)
  {
    printf("%s - A terminal based calculator\n", programName);
    printf("Usage: %s <command> [OPTIONS]\n", programName);
    printf("\n");
    printf("Execute simple to more complex math expressions in the terminal.\n");
    printf("\n");
    printf("Example:\n");
    printf(">>> %s input \"10.5 + 30 - sqrt(PI * 5.2) / 8\"\n", programName);
    printf("\n");

    print_available_commands(stdout, programName, deprecated);
    printf("\n");

    int max_width = 0;

    // Prints program inforamtions.
    printf("Supported operators [USAGE, NAME]:\n");
    for (size_t i = 0; i < OP_COUNT; ++i)
      printf("    '%-*c'   %s\n", 1, operatorTypeIdentifiers[i], operatorTypeNames[i]);
    printf("\n");

    printf("Supported functions [USAGE(x), DESCRIPTION]:\n");
    for (size_t i = 0; i < FT_COUNT; ++i)
    {
      int width = strlen(functionTypeIdentifiers[i]);
      if (width > max_width)
        max_width = width;
    }
    for (size_t i = 0; i < FT_COUNT; ++i)
      printf("    %-*s   %s\n", max_width, functionTypeIdentifiers[i], functionTypeDescriptions[i]);
    printf("\n");

    printf("Supported math-constants [USAGE, NAME]:\n");
    max_width = 0;
    for (size_t i = 0; i < MC_COUNT; ++i)
    {
      int width = strlen(mathConstantTypeIdentifiers[i]);
      if (width > max_width)
        max_width = width;
    }
    for (size_t i = 0; i < MC_COUNT; ++i)
      printf("    %-*s   %s\n", max_width, mathConstantTypeIdentifiers[i], mathConstantTypeNames[i]);
    printf("\n");

    // TODO: Implement later when cli mode is implemented!
    // print_global_program_config_help();
    // printf("\n");

    printf("Full documentation <" FULL_DOCUMENTATION_LINK ">\n");
    return true;
  }

  const char *command_name = flag_shift_args(&argc, &argv);

  for (size_t i = 0; i < ARRAY_LEN(commands); ++i)
  {
    command_t *current = &commands[i];

    if (strcmp(command_name, current->name) == 0)
    {
      char *help_flag = "-h";

      if (!current->run(current, programName, 1, &help_flag))
        return false;
      return true;
    }
  }

  // Error
  print_available_commands(stderr, programName, deprecated);

  fprintf(stderr, "\n");
  fprintf(stderr, "ERROR: Unknown command `%s`\n", command_name);

  return false;
}

static bool test_run(command_t *self, const char *programName, int argc, char **argv)
{
  bool verbose = false;
  bool help = false;

  void *c = flag_c_new(programName);
  flag_c_bool_var(c, &help, "h", false, "Print this help message");
  flag_c_bool_var(c, &verbose, "v", NULL, "Execute all tests with verbose logging");

  if (!flag_c_parse(c, argc, argv))
  {
    print_command_usage(stderr, self, programName, c);
    flag_c_print_error(c, stderr);
    return false;
  }

  argc = flag_c_rest_argc(c);
  argv = flag_c_rest_argv(c);

  if (help)
  {
    print_command_usage(stdout, self, programName, c);
    return true;
  }

  if (argc != 0)
  {
    print_command_usage(stderr, self, programName, c);
    fprintf(stderr, "ERROR: Too many parameters given\n");
    return false;
  }

  flag_c_free(c);

  arena_t arena = {0};
  bool freeAfterEachTest = false;

  node_func_args_t test2ArgsList1 = {0};
  node_t *test2ArgsList1Item1 = node_constant(&arena, 0, 10);
  node_func_args_append(&arena, &test2ArgsList1, test2ArgsList1Item1);

  node_func_args_t test3ArgsList1 = {0};
  node_t *test3ArgsList1Item1 = node_binop(&arena, 0, NO_SUB,
                                           node_constant(&arena, 0, 3.5),
                                           node_constant(&arena, 0, mathConstantTypeValues[MC_EULERS_NUMBER]));
  node_func_args_append(&arena, &test3ArgsList1, test3ArgsList1Item1);

  node_func_args_t test3ArgsList2 = {0};
  node_t *test3ArgsList2Item1 = node_binop(&arena, 0, NO_MUL,
                                           node_constant(&arena, 0, 44.23),
                                           node_binop(&arena, 0, NO_POW,
                                                      node_constant(&arena, 0, 6.4),
                                                      node_constant(&arena, 0, 2)));
  node_func_args_append(&arena, &test3ArgsList2, test3ArgsList2Item1);

  node_func_args_t test3ArgsList3 = {0};
  node_t *test3ArgsList3Item1 = node_constant(&arena, 0, 10);
  node_func_args_append(&arena, &test3ArgsList3, test3ArgsList3Item1);

  node_func_args_t test3ArgsList4 = {0};
  node_t *test3ArgsList4Item1 = node_binop(&arena, 0, NO_POW,
                                           node_constant(&arena, 0, 5),
                                           node_constant(&arena, 0, mathConstantTypeValues[MC_EULERS_CONSTANT]));
  node_func_args_append(&arena, &test3ArgsList4, test3ArgsList4Item1);

  node_func_args_t test4ArgsList1 = {0};
  node_t *test4ArgsList1Item1 = node_constant(&arena, 0, 4);
  node_func_args_append(&arena, &test4ArgsList1, test4ArgsList1Item1);

  node_test_t tests[] = {
      // Test 1
      {
          // IN: "1 + 2 + (PI ^ 2) / 3"
          // AST: add(1.00000, add(2.00000, divide(paren(pow(3.14159, 2.00000)), 3.00000)))
          // = 6.28986
          .inputExpression = "1 + 2 + (PI ^ 2) / 3",
          .expectedResult = 6.28986,
          .expressionTree = node_binop(&arena, 0, NO_ADD,
                                       node_constant(&arena, 0, 1),
                                       node_binop(&arena, 0, NO_ADD,
                                                  node_constant(&arena, 0, 2),
                                                  node_binop(&arena, 0, NO_DIV,
                                                             node_paren(&arena, 0,
                                                                        node_binop(&arena, 0, NO_POW,
                                                                                   node_constant(&arena, 0, mathConstantTypeValues[MC_PI]),
                                                                                   node_constant(&arena, 0, 2))),
                                                             node_constant(&arena, 0, 3)))),
      },
      // Test 2
      {
          // IN: "100 + ln(10)"
          // AST: add(100.0000, ln(10))
          // = 102.30258
          .inputExpression = "100 + ln(10)",
          .expectedResult = 102.30258,
          .expressionTree = node_binop(&arena, 3, NO_ADD,
                                       node_constant(&arena, 0, 100),
                                       node_func(&arena, 4, NF_LN, test2ArgsList1)),
      },
      // Test 3
      {
          // IN: "100.53 + sqrt(3.5 - EN) + cos(44.23 * 6.4^2) / 8.3 + ln(10) - PI + ln(5^EC)"
          // AST: add(100.53000, add(sqrt(substract(3.50000, 2.71828)), add(divide(acos(multiply(44.23000, pow(6.40000, 2.00000))), 8.30000), substract(ln(10.00000), add(3.14159, ln(pow(5.00000, 0.57722)))))))
          // = 99.58497
          .inputExpression = "100.53 + sqrt(3.5 - EN) + cos(44.23 * 6.4^2) / 8.3 + ln(10) - PI + ln(5^EC)",
          .expectedResult = 99.58497,
          .expressionTree = node_binop(&arena, 0, NO_ADD,
                                       node_constant(&arena, 0, 100.53),
                                       node_binop(&arena, 0, NO_ADD,
                                                  node_func(&arena, 0, NF_SQRT, test3ArgsList1),
                                                  node_binop(&arena, 0, NO_ADD,
                                                             node_binop(&arena, 0, NO_DIV,
                                                                        node_func(&arena, 0, NF_COS, test3ArgsList2),
                                                                        node_constant(&arena, 0, 8.3)),
                                                             node_binop(&arena, 0, NO_SUB,
                                                                        node_func(&arena, 0, NF_LN, test3ArgsList3),
                                                                        node_binop(&arena, 0, NO_ADD,
                                                                                   node_constant(&arena, 0, mathConstantTypeValues[MC_PI]),
                                                                                   node_func(&arena, 0, NF_LN, test3ArgsList4)))))),
      },
      // Test 4
      {
          // IN: "10.5 * exp(4)"
          // AST: mult(10.5, exp(4))
          // = 573.28058
          .inputExpression = "10.5 * exp(4)",
          .expectedResult = 573.28058,
          .expressionTree = node_binop(&arena, 0, NO_MUL,
                                       node_constant(&arena, 0, 10.5),
                                       node_func(&arena, 0, NF_EXP, test4ArgsList1)),
      },
      // Test 5
      {
          // IN: "10 + 5 / (5 * 0)"
          // AST: "add(10, divide(5, paren(mult(5, 0))))"
          // = ERR (Divide by Zero)
          .inputExpression = "10 + 5 / (5 * 0)",
          .shouldFail = true,
          .expressionTree = node_binop(&arena, 3, NO_ADD,
                                       node_constant(&arena, 0, 10),
                                       node_binop(&arena, 6, NO_DIV,
                                                  node_constant(&arena, 4, 5),
                                                  node_paren(&arena, 8,
                                                             node_binop(&arena, 10, NO_MUL,
                                                                        node_constant(&arena, 9, 5),
                                                                        node_constant(&arena, 11, 0))))),
      },
      // Test 6
      {
          // IN: "(5 * 0)"
          // AST: "paren(mult(5, 0))"
          // = 0
          .inputExpression = "(5 * 0)",
          .expectedResult = 0,
          .expressionTree = node_paren(&arena, 0,
                                       node_binop(&arena, 3, NO_MUL,
                                                  node_constant(&arena, 1, 5),
                                                  node_constant(&arena, 5, 0))),
      },
      // Test 7
      {
          // IN: "10 / 0"
          // AST: "div(10, 0)"
          // = ERR (Divide by Zero)
          .inputExpression = "10 / 0",
          .shouldFail = true,
          .expressionTree = node_binop(&arena, 2, NO_DIV,
                                       node_constant(&arena, 0, 10),
                                       node_constant(&arena, 4, 0)),
      },
      // Test 8
      {
          // IN: "10 / (4)"
          // AST: "div(10, paren(4))"
          // = 2.5
          .inputExpression = "10 / (4)",
          .expectedResult = 2.5,
          .expressionTree = node_binop(&arena, 3, NO_DIV,
                                       node_constant(&arena, 0, 10),
                                       node_paren(&arena, 5,
                                                  node_constant(&arena, 6, 4))),
      },
      // Test 9
      {
          // IN: "(2+2)*8/2"
          // AST "divide(mult(par(add(2,2)),8),2)"
          // = 16
          .inputExpression = "(2+2)*8/2",
          .expectedResult = 16.0,
          .expressionTree = node_binop(&arena, 8, NO_DIV,
                                       node_binop(&arena, 6, NO_MUL,
                                                  node_paren(&arena, 1,
                                                             node_binop(&arena, 3, NO_ADD,
                                                                        node_constant(&arena, 2, 2),
                                                                        node_constant(&arena, 4, 2))),
                                                  node_constant(&arena, 2, 8)),
                                       node_constant(&arena, 2, 2)),
      },
      // Test 10
      {
          // IN: "8/2*(2+2)"
          // AST "divide(8,mult(2,par(add(2,2))))"
          // = 16
          .inputExpression = "8/2*(2+2)",
          .expectedResult = 16.0,
          .expressionTree = node_binop(&arena, 4, NO_MUL,
                                       node_binop(&arena, 2, NO_DIV,
                                                  node_constant(&arena, 1, 8),
                                                  node_constant(&arena, 2, 2)),
                                       node_paren(&arena, 5,
                                                  node_binop(&arena, 7, NO_ADD,
                                                             node_constant(&arena, 6, 2),
                                                             node_constant(&arena, 8, 2)))),
      },
  };

  printf("Executing all tests:\n\n");

  const size_t totalTests = ARRAY_LEN(tests);
  size_t failedTests = 0;

  for (size_t i = 0; i < totalTests; ++i)
  {
    printf("Test %zu:\n", i + 1);
    if (!execute_test(&arena, &tests[i], verbose, freeAfterEachTest))
      ++failedTests;
    printf("\n");
  }

  // Final cleanup
  if (!freeAfterEachTest)
    arena_free(&arena);

  printf("Tests executed: %zu\n", totalTests);
  printf("Successful tests: %zu\n", totalTests - failedTests);
  printf("Failed tests: %zu\n", failedTests);

  return failedTests != 0;
}

int main(int argc, char **argv)
{
  char *programName = flag_shift_args(&argc, &argv);

  // Store only the current program name without path: './bin/PROGRAM_NAME' -> 'PROGRAM_NAME'
  cstr_chop_till_last_delim(&programName, '/');

  if (argc <= 0)
  {
    print_available_commands(stderr, programName, false);
    fprintf(stderr, "\n");
    fprintf(stderr, "ERROR: No command is provided!\n");
    return 0;
  }

  const char *command_name = flag_shift_args(&argc, &argv);

  for (size_t i = 0; i < ARRAY_LEN(commands); ++i)
  {
    if (strcmp(command_name, commands[i].name) == 0)
    {
      command_t *c = &commands[i];
      return c->run(c, programName, argc, argv);
    }
  }

  print_available_commands(stderr, programName, false);

  fprintf(stderr, "\n");
  fprintf(stderr, "ERROR: Unknown command `%s`\n", command_name);

  return 1;
}
