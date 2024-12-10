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

const char* globalProgramModeNames[GPM_COUNT] = {
  [GPM_SINGLE_CLI_EXPRESSION_ARG] = "CLI argument",
  [GPM_FULL_CLI]                  = "Full CLI",
  [GPM_EXPRESSION_FILE]           = "Standard math file mode",
  [GPM_LINKER_FILE]               = "Linker math file mode",
};

const char* globalProgramModeDescriptions[GPM_COUNT] = {
  [GPM_SINGLE_CLI_EXPRESSION_ARG] = "This mode is used when using an expression as an cli argument. For simple direct execution of an expression given to the program as an argument. In this mode only the simple base features are active. No assignments or multi line inputs, just simple expressions. Custom functions or variables can still be linked by file if they are needed in the expression.",
  [GPM_FULL_CLI]                  = "This mode is used when starting without specifying any arguments like expression or file. Linking is allowed to link to pre defined definitions while writing and evaluating expressions. With this mode the program stays open and expects an input expression per line entered. Here are the most features active like variable and function assignments.",
  [GPM_EXPRESSION_FILE]           = "This mode is used when an expression file is given as an argument. Linking is also allowed so that custom external definitions can be used in the expression file. When the program was started with a file given as an argument. With this mode the program parses the given file line by line and evaluates it. In this mode is every feature usable like function and variable assignments and new lines to break a single line into multiple for easier readability.",
  [GPM_LINKER_FILE]               = "Similar like the expression file but this mode does only allow for variable and function definitions but no direct evaluation. These files are like libraries for often used custom math functions and custom variables and will get loaded into the program on startup before executing a given expression so the user can use the definitions in the linked files in it."
};



// TODO: Rethink if needed.
// It defaults to the single expression mode used when entering an expression as an cli argument.
// This variable is normally not needed outside of this context because the only thing the program needs to know are the currently available features for a specific context.
static e_global_program_mode _globalProgramMode = GPM_SINGLE_CLI_EXPRESSION_ARG;


// TODO: Rethink!
// Used for changing the program mode.
void change_global_program_mode(e_global_program_mode mode)
{
  assert(mode < GPM_COUNT);
  if (_globalProgramMode == mode) return;
  _globalProgramMode = mode;
}



// This stores the feature config.
typedef struct {
  bool expression_eval_allowed;
  bool comments_allowed;
  bool new_lines_allowed;
  bool variable_definitions_allowed;
  bool function_definitions_allowed;
} program_mode_config_t;

// EXPRESSION-EVAL, COMMENTS, NEW-LINES, VARIABLE-DEFINITIONS, FUNCTION-DEFINITIONS
#define _NEW_PMC(eea, ca, nla, vda, fda)    \
  {                                         \
    .expression_eval_allowed = (eea),       \
    .comments_allowed = (ca),               \
    .new_lines_allowed = (nla),             \
    .variable_definitions_allowed = (vda),  \
    .function_definitions_allowed = (fda)   \
  }


// All configurations for every program mode.
static const program_mode_config_t globalProgramModeConfigurations[GPM_COUNT] = {
  [GPM_SINGLE_CLI_EXPRESSION_ARG] = _NEW_PMC(true, false, false, false, false),
  [GPM_FULL_CLI]                  = _NEW_PMC(true, false, false, true, true),
  [GPM_EXPRESSION_FILE]           = _NEW_PMC(true, true, true, true, true),
  [GPM_LINKER_FILE]               = _NEW_PMC(false, true, true, true, true),
};


// Needs to be in the same layout as the 'program_mode_config_t'.
typedef enum {
  SPMC_EXPR_EVAL_ALLOWED,
  SPMC_COMMENTS_ALLOWED,
  SPMC_NEW_LINES_ALLOWED,
  SPMC_VAR_DEF_ALLOWED,
  SPMC_FUNC_DEF_ALLOWED,

  SPMC_COUNT
} e_specific_program_mode_config;

static_assert(SPMC_COUNT == 5, "Amount of specific-program-mode-config has changed");

const char* specificProgramModeConfigNames[SPMC_COUNT] = {
  [SPMC_EXPR_EVAL_ALLOWED]  = "Expression evaluation allowed",
  [SPMC_COMMENTS_ALLOWED]   = "Comments allowed",
  [SPMC_NEW_LINES_ALLOWED]  = "New lines allowed",
  [SPMC_VAR_DEF_ALLOWED]    = "Variable definitions allowed",
  [SPMC_FUNC_DEF_ALLOWED]   = "Function definitions allowed",
};

const char* specificProgramModeConfigDescriptions[SPMC_COUNT] = {
  [SPMC_EXPR_EVAL_ALLOWED]  = "Are expressions allowed to be evaluated. In a standard file the final expression which should get evaluated which uses all custom variables or functions is written in the last line like '= EXPRESSION'. This is f.e. not allowed in linker files. Expressions can also be written in the simple mode when starting with an expression argument or in the full cli mode. But in these the expression gets written without '=' at the start because it gets evaluated directly. The standard file uses that to know what expression should get evaluated. A standard file can have one or multiple evaluation definitions to support multiple evaluations from the same file at once.",
  [SPMC_COMMENTS_ALLOWED]   = "Are comments allowed. Comments are defined with '//' and get used like '... // YOUR COMMENT GOES HERE'. When defining a comment everything after the definition till the end of the line will get ignored.",
  [SPMC_NEW_LINES_ALLOWED]  = "Is it allowed to split a single expression line into multiple to make it more readable when the expression gets too long. A single line can be split with '\\' at the end. This must be placed on every line of the same expression except the last.",
  [SPMC_VAR_DEF_ALLOWED]    = "Are variable definitions allowed. Custom variables are used like pre defined math-constants like f.e. 'PI' and allow for wrapping long expressions into variables which can be reused everywhere to make expressions more compact. Variables are defined like 'VAR_NAME = EXPRESSION'. Variable names must be globally unique in all contexts and must also not be named like the pre defined math constants.",
  [SPMC_FUNC_DEF_ALLOWED]   = "Are function definitions allowed. Custom functions are used like pre defined functions like f.e. 'sqrt' and allow for wrapping an expression inside a variable with a list of arguments. These arguments can be used inside the expression like variables to easily change values and reuse the expression. Functions are defined like 'FUNC_NAME(ARG1, ARG2, ...) = EXPRESSION'. Function names must be globally unique in all contexts and must also not be named like the pre defined functions like 'sqrt' or 'ln'.",
};



// TODO: Rethink!
// Loads a specifc configuration from the currently set program mode.
bool get_current_specific_program_config_ex(e_global_program_mode mode, e_specific_program_mode_config specificConfig)
{
  // TODO: Rethink! When feeling a bit funny this can be used.
  // return (bool) ((&globalProgramModeConfigurations[mode]) + sizeof(bool) * specificConfig);
  
  // TODO: Rethink!
  // Loads the current config from the currently set global program mode.
  //program_mode_config_t* currentConfig = &globalProgramModeConfigurations[mode];
  program_mode_config_t currentConfig = globalProgramModeConfigurations[mode];

  switch (specificConfig)
  {
    case SPMC_EXPR_EVAL_ALLOWED: return currentConfig.expression_eval_allowed;
    case SPMC_COMMENTS_ALLOWED:  return currentConfig.comments_allowed;
    case SPMC_NEW_LINES_ALLOWED: return currentConfig.new_lines_allowed;
    case SPMC_VAR_DEF_ALLOWED:   return currentConfig.variable_definitions_allowed;
    case SPMC_FUNC_DEF_ALLOWED:  return currentConfig.function_definitions_allowed;
    case SPMC_COUNT:
    default:
      UNREACHABLE("Not implemented!");
  }

  return false;
}

#define get_current_specific_program_config(specificConfig) get_current_specific_program_config_ex(_globalProgramMode, (specificConfig))


// TODO: Rethink and test!
void print_specific_program_config(e_global_program_mode mode)
{
  printf("Specific config:\n");

  for (size_t i = 0; i < SPMC_COUNT; ++i)
  {
    printf("%-40s %s\n", specificProgramModeConfigNames[i], get_current_specific_program_config_ex(mode, i) ? "true" : "false");
  }
}


// TODO: Rethink and test!
void print_global_program_config_help()
{
  printf("Configurations and descriptions:\n\n");

  printf("Configuration descriptions:\n");
  for (size_t i = 0; i < SPMC_COUNT; ++i)
  {
    // TODO: Rethink!
    printf("%-40s %s\n", specificProgramModeConfigNames[i], specificProgramModeConfigDescriptions[i]);
  }

  printf("\n");
  
  for (size_t i = 0; i < GPM_COUNT; ++i)
  {
    printf("%-40s %s\n", globalProgramModeNames[i], globalProgramModeDescriptions[i]);
    print_specific_program_config((e_global_program_mode) i);
    printf("\n");
  }
}

#endif // _CONFIG_H_
