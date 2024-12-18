#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "helpers.h"


// TODO: Rethink!
/*
 Will be used for the parsing of expressions in different modes.
 The mode depends on the usage of the program, because f.e. variables or function definitions can't be used when
 executing a single expression directly in the program arguments. Thats because the program exits directly after evaluating
 the given expression and does not store state in this mode. But when the full cli mode is used where the program was started
 without any extra arguments except f.e. 'verbose' for executing and handling the expressions, it will stay open and multiple
 lines can be entered where some lines could be assignments or func definitions and some are expressions which use them. So in
 full cli mode it stores the previously entered data. Also the full cli mode is used when starting the program with a file
 where all this is pre written in multiple lines and the program just prints the final value of the evaluated expression.
 
 Also rethink if a linker should be implemented when implementing files, so often used functions can be pre written
 in one or multiple files, which only contain variable- and function-definitions for easy reusability.
 This would make it possible to use more complex and custom variables and functions in f.e single-mode where only the fewest features
 are active. The variables can also be used because they would work exactly the same way as the pre defined math-constants. Same with
 functions, because they also work the same way like the pre defined functions.
 The linking could be used with a specific cli command like '-l <FILE>' or '--link <FILE>' and could allow multiple calls for linking
 one or many different files.
 The linker than loads everything in memory and pre parses all definitions, so that the user input can be combined like it would be just
 a single file.

 Evaluation-File Example:
 """
 var1 = 10 + sqrt(10 ^ EN)

 func1(x1, x2) = var1 * x1 ^ ln(x2 ^ 2)
 
 = func1(10, -4 * PI) - ln(20)
 """

 Linker-File Example:
 """
 var1 = 100 / sqrt(20 ^ 2 - 5)

 funcTest(arg1, arg2) = arg1 + 20 / ln(arg2)
 """

 Retink if '\' should be implemented for easily splitting a single lines into multiple for larger expressions.
 New lines can also only be used in the full cli mode.
 New-Line Example:
 """
 func1(x1, x2) = \
  100.53 + sqrt(3.5 - EN) + \
  cos(x1 * 6.4^2) / 8.3 + ln(10) - \
  PI + ln(5 ^ x2)
 """
*/


typedef enum {
  GPM_SINGLE_CLI_EXPRESSION_ARG,
  GPM_FULL_CLI,
  GPM_EXPRESSION_FILE,
  GPM_LINKER_FILE,

  GPM_COUNT
} e_global_program_mode;

static_assert(GPM_COUNT == 4, "Amount of global-program-modes have changed");

#define ASSERT_PROGRAM_MODE(mode) assert((mode) < GPM_COUNT && "Invalid program-mode!")

static const char* globalProgramModeNames[GPM_COUNT] = {
  [GPM_SINGLE_CLI_EXPRESSION_ARG] = "Simple CLI expr-argument mode",
  [GPM_FULL_CLI]                  = "Full CLI mode",
  [GPM_EXPRESSION_FILE]           = "Standard-Expr-File mode",
  [GPM_LINKER_FILE]               = "Linker-Expr-File mode",
};

static const char* globalProgramModeDescriptions[GPM_COUNT] = {
  [GPM_SINGLE_CLI_EXPRESSION_ARG] =
    "  This mode is used when using an expression as an cli argument.\n"
    "  For simple direct execution of an expression given to the program\n"
    "  as an argument. In this mode only the simple base features are active.\n"
    "  No assignments or multi line inputs, just simple expressions.\n"
    "  Custom functions or variables can still be linked by file if they\n"
    "  are needed in the expression.",
  
  [GPM_FULL_CLI] =
    "  This mode is used when starting without specifying any arguments\n"
    "  like expression or file. Linking is allowed to link to pre defined\n"
    "  definitions while writing and evaluating expressions. With this mode\n"
    "  the program stays open and expects an input expression per line entered.\n"
    "  Here are the most features active like variable and function assignments.",
  
  [GPM_EXPRESSION_FILE] =
    "  This mode is used when an expression file is given as an argument.\n"
    "  Linking is also allowed so that custom external definitions can be\n"
    "  used in the expression file. When the program was started with a file\n"
    "  given as an argument. With this mode the program parses the given file\n"
    "  line by line and evaluates it. In this mode is every feature usable like\n"
    "  function and variable assignments and new lines to break a single line\n"
    "  into multiple for easier readability.",
  
  [GPM_LINKER_FILE] =
    "  Similar like the expression file but this mode does only allow for\n"
    "  variable and function definitions but no direct evaluation.\n"
    "  These files are like libraries for often used custom math functions\n"
    "  and custom variables and will get loaded into the program on startup\n"
    "  before executing a given expression so the user can use the\n"
    "  definitions in the linked files in it."
};



// TODO: Rethink if needed.
// It defaults to the single expression mode used when entering an expression as an cli argument.
// This variable is normally not needed outside of this context because the only thing the programn
// needs to know are the currently available features for a specific context.
// ---
// It could be set multiple times while running depending on the given arguments when starting.
// Like when starting with an cli arg expression and with a linked file:
// 1. Checks that linked files are given as args and an expression in the cli args.
// 2. Sets this flag to linked file and handle everything needed for the given file.
// 3. Linked data is in memory.
// 4. This flag gets set to single cli expression which is needed for the cli expression argument.
// 5. Handle the given expression expression in that mode and evaluate it.
static e_global_program_mode _globalProgramMode = GPM_SINGLE_CLI_EXPRESSION_ARG;


// Used for changing the program mode.
void change_global_program_mode(e_global_program_mode mode)
{
  ASSERT_PROGRAM_MODE(mode);
  if (_globalProgramMode == mode) return;
  _globalProgramMode = mode;
}



typedef enum {
  CF_EXPRESSION_EVALUATION_ALLOWED  = (1u << 0),
  CF_COMMENTS_ALLOWED               = (1u << 1),
  CF_NEW_LINES_ALLOWED              = (1u << 2),
  CF_VARIABLE_DEFINITIONS_ALLOWED   = (1u << 3),
  CF_FUNCTION_DEFINITIONS_ALLOWED   = (1u << 4),
} e_config_flags;

// Needs to be in the same layout as the 'e_config_flags'.
typedef enum {
  SPMC_EXPR_EVAL_ALLOWED,
  SPMC_COMMENTS_ALLOWED,
  SPMC_NEW_LINES_ALLOWED,
  SPMC_VAR_DEF_ALLOWED,
  SPMC_FUNC_DEF_ALLOWED,

  SPMC_COUNT
} e_specific_program_mode_config;

static_assert(SPMC_COUNT == 5, "Amount of specific-program-mode-config has changed");

#define ASSERT_SPECIFIC_CONFIG(specificConfig) assert((specificConfig) < SPMC_COUNT && "Invalid config!")


static inline e_config_flags specific_prog_mode_conf_to_config_flag(e_specific_program_mode_config config)
{
  ASSERT_SPECIFIC_CONFIG(config);

  return (e_config_flags) (1u << (int)config);
}


static const char* specificProgramModeConfigNames[SPMC_COUNT] = {
  [SPMC_EXPR_EVAL_ALLOWED]  = "Expression-Evaluation",
  [SPMC_COMMENTS_ALLOWED]   = "Comments",
  [SPMC_NEW_LINES_ALLOWED]  = "New-Lines",
  [SPMC_VAR_DEF_ALLOWED]    = "Variable-Definitions",
  [SPMC_FUNC_DEF_ALLOWED]   = "Function-Definitions",
};

static const char* specificProgramModeConfigDescriptions[SPMC_COUNT] = {
  [SPMC_EXPR_EVAL_ALLOWED] =
    "  Are expressions allowed to be evaluated.\n"
    "  In a standard file the final expression which should get evaluated\n"
    "  which uses all custom variables or functions is written in the last\n"
    "  line like '= EXPRESSION'. This is f.e. not allowed in linker files.\n"
    "  Expressions can also be written in the simple mode when starting with\n"
    "  an expression argument or in the full cli mode. But in these the\n"
    "  expression gets written without '=' at the start because it gets\n"
    "  evaluated directly. The standard file uses that to know what expression\n"
    "  should get evaluated. A standard file can have one or multiple evaluation\n"
    "  definitions to support multiple evaluations from the same file at once.",
  
  [SPMC_COMMENTS_ALLOWED] =
    "  Are comments in the input allowed.\n"
    "  Comments are defined with '//' and get used like\n"
    "  '... // YOUR COMMENT GOES HERE'. When defining a comment everything\n"
    "  after the definition till the end of the line will get ignored.",
  
  [SPMC_NEW_LINES_ALLOWED] =
    "  Is it allowed to split a single expression line into multiple to make\n"
    "  it more readable when the expression gets too long. A single line can\n"
    "  be split with '\\' at the end. This must be placed on every line of the\n"
    "  same expression except the last.",
  
  [SPMC_VAR_DEF_ALLOWED] =
    "  Are variable definitions allowed.\n"
    "  Custom variables are used like pre defined math-constants like f.e. 'PI'\n"
    "  and allow for wrapping long expressions into variables which can be reused\n"
    "  everywhere to make expressions more compact. Variables are defined like\n"
    "  'VAR_NAME = EXPRESSION'. Variable names must be globally unique in all\n"
    "  contexts and must also not be named like the pre defined math constants.",
  
  [SPMC_FUNC_DEF_ALLOWED] =
    "  Are function definitions allowed.\n"
    "  Custom functions are used like pre defined functions like f.e. 'sqrt' and\n"
    "  allow for wrapping an expression inside a variable with a list of arguments.\n"
    "  These arguments can be used inside the expression like variables to easily\n"
    "  change values and reuse the expression. Functions are defined like\n"
    "  'FUNC_NAME(ARG1, ARG2, ...) = EXPRESSION'. Function names must be globally\n"
    "  unique in all contexts and must also not be named like the pre defined\n"
    "  functions like 'sqrt' or 'ln'.",
};


// The specific configurations for each program mode.
static const e_config_flags globalProgramModeConfigurations[GPM_COUNT] =
{
  [GPM_SINGLE_CLI_EXPRESSION_ARG] = CF_EXPRESSION_EVALUATION_ALLOWED,

  [GPM_FULL_CLI]                  = CF_EXPRESSION_EVALUATION_ALLOWED  |
                                    CF_VARIABLE_DEFINITIONS_ALLOWED   |
                                    CF_FUNCTION_DEFINITIONS_ALLOWED,
  
  [GPM_EXPRESSION_FILE]           = CF_EXPRESSION_EVALUATION_ALLOWED  |
                                    CF_COMMENTS_ALLOWED               |
                                    CF_NEW_LINES_ALLOWED              |
                                    CF_VARIABLE_DEFINITIONS_ALLOWED   |
                                    CF_FUNCTION_DEFINITIONS_ALLOWED,
  
  [GPM_LINKER_FILE]               = CF_COMMENTS_ALLOWED               |
                                    CF_NEW_LINES_ALLOWED              |
                                    CF_VARIABLE_DEFINITIONS_ALLOWED   |
                                    CF_FUNCTION_DEFINITIONS_ALLOWED,
};


// Loads a specifc configuration from the currently set program mode.
bool get_specific_program_config_ex(e_global_program_mode mode, e_specific_program_mode_config specificConfig)
{
  ASSERT_PROGRAM_MODE(mode);
  ASSERT_SPECIFIC_CONFIG(specificConfig);

  return is_bit_set(globalProgramModeConfigurations[mode], specific_prog_mode_conf_to_config_flag(specificConfig));
}

bool get_current_specific_program_config(e_specific_program_mode_config specificConfig)
{
  return get_specific_program_config_ex(_globalProgramMode, specificConfig);
}



void print_global_program_config_help()
{
  printf("Configurations and mode descriptions:\n\n");

  printf("Configuration descriptions:\n\n");

  for (size_t configIdx = 0; configIdx < SPMC_COUNT; ++configIdx)
  {
    printf("%s\n", specificProgramModeConfigNames[configIdx]);
    printf("%s\n", specificProgramModeConfigDescriptions[configIdx]);
    if (configIdx < SPMC_COUNT - 1) printf("\n");
  }

  printf("\n");
  
  printf("Configurations by mode:\n\n");

  for (size_t pModeIdx = 0; pModeIdx < GPM_COUNT; ++pModeIdx)
  {
    printf("%s\n", globalProgramModeNames[pModeIdx]);
    printf("%s\n", globalProgramModeDescriptions[pModeIdx]);

    // Prints all configurations for a specific program-mode.
    for (size_t configIdx = 0; configIdx < SPMC_COUNT; ++configIdx)
    {
      bool configFlag = get_specific_program_config_ex((e_global_program_mode) pModeIdx, (e_specific_program_mode_config) configIdx);
      printf("  - %-35s%s\n", specificProgramModeConfigNames[configIdx], bool_cstr(configFlag));
    }

    printf("\n");
  }
}

#endif // _CONFIG_H_
