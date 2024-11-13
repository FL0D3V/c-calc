# TODO

## Main functionality

- [X] Tokenizer: Converts the input string to valid tokens and checks for invalid characters. Input string must be e.g.: "100.53 + 23 * (53.21 - 34) / 2.5".
- [X] Lexer: Is an extension to the tokenizer which adds additional meta data to the tokens like what type of token and extra values like converted value literal or operator type.
- [X] AST: Implement the AST representation of all token types.
- [X] Evaluator: Evaluates the AST to get the final result value.
- [ ] Parser: Converts the lexed tokens to an AST (abstract syntax tree) for checking e.g. the "order of operations" of the math equation.
- [X] Simple user-friendly CLI.
- [ ] Complex CLI interface with the possibility to enter multiple math expressions without having to restart the program.

## Missing extras

- [ ] Implement ',' for functions with multiple parameters.
- [X] 'Peek' and 'Seek' functions for the tokenization (not really needed with current implementation of it but propably needed for better tokenization)
- [X] Handling of float values (not like currently with an extra 'Comma' token but with a different number type literal in the unit f.e.)
- [ ] Arena Allocator for memory management
- [ ] Variables and variable assignments and storing variables while running for e.g. multi line math expressions.
- [ ] Solving math expressions with variables and assignments.
- [ ] Maybe implement custom functions like variables that take one or multiple parameters/variables.

## Math-Functions

- [X] acos(x)
  Returns the arc cosine of x in radians.

- [X] asin(x)
  Returns the arc sine of x in radians.

- [X] atan(x)
  Returns the arc tangent of x in radians.

- [ ] atan2(y, x)
  Returns the arc tangent in radians of y/x based on the signs of both values to determine the correct quadrant.

- [X] cos(x)
  Returns the cosine of a radian angle x.

- [X] cosh(x)
  Returns the hyperbolic cosine of x.

- [X] sin(x)
  Returns the sine of a radian angle x.

- [X] sinh(x)
  Returns the hyperbolic sine of x.

- [X] tan(x)
  Returns the tangent of a given angle x.

- [X] tanh(x)
  Returns the hyperbolic tangent of x.

- [X] exp(x)
  Returns the value of e raised to the x'th power.

- [ ] ldexp(x, exponent: int)
  Returns x multiplied by 2 raised to the power of exponent.

- [X] log(x)
  Returns the natural logarithm (base-e logarithm) of x.

- [X] log10(x)
  Returns the common logarithm (base-10 logarithm) of x.

- [X] sqrt(x)
  Returns the square root of x.
