#include "program.h"

int main(int argc, char** argv)
{
  // TODO: Implement full CLI interface mode if the user does not run the program with a math expression directly.
  // This could then be used to support variables and solving for variables and store full math expressions with variables in memory for later
  // solving. Could be used to store a few base expressions into variables and then using them to calulate more complex expressions.

  program_t prog = validate_cli_input(argc, argv);
  
  return handle_program(&prog);
}
