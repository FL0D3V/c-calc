// TODO: Rethink!
#define COMPILER_VERSION "gcc version 14.2.0 (Debian 14.2.0-6)"
#define GIT_HASH "c1b2777"
#define BUILD_TIME "2026-10-23 03:12:00"

#include "program.h"

int main(int argc, char **argv)
{
  // TODO: Implement full CLI interface mode if the user does not run the program with a math expression directly.
  // This could then be used to support variables and solving for variables and store full math expressions with variables in memory for later
  // solving. Could be used to store a few base expressions into variables and then using them to calulate more complex expressions.

  // program_t prog = validate_cli_input(argc, argv);

  // return handle_program(&prog);

  // TODO: Rethink!
  const char *program_name = flag_shift_args(&argc, &argv);

  if (argc <= 0)
  {
    print_available_commands(stderr, program_name, false);
    fprintf(stderr, "\n");
    fprintf(stderr, "ERROR: No command is provided\n");
    return 0;
  }

  const char *command_name = flag_shift_args(&argc, &argv);

  for (size_t i = 0; i < ARRAY_LEN(commands); ++i)
  {
    if (strcmp(command_name, commands[i].name) == 0)
    {
      if (!commands[i].run(&commands[i], program_name, argc, argv))
        return 1;
      return 0;
    }
  }

  print_available_commands(stderr, program_name, false);
  fprintf(stderr, "\n");
  fprintf(stderr, "ERROR: Unknown command `%s`\n", command_name);
  return 1;
}
