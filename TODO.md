# TODO

## Main functionality

- [X] Tokenizer: Converts the input string to valid tokens and checks for invalid characters. Input string must be e.g.: "100.53 + PI * (53.21 - 34) / 2.5 + sqrt(5 ^ EN)".
- [X] Implement the usage of negative numbers like e.g. '5 * -3'.
- [X] Lexer: Is an extension to the tokenizer which adds additional meta data to the tokens like what type of token and extra values like converted value literal or operator type. Also it checks for invalid tokens.
- [X] AST: Implement the AST with nodes.
- [X] Evaluator: Evaluates the AST to get the final result value.
- [X] Semantics checking: Checks if the input (lexed tokens) has correct syntax.
- [ ] Parser: Converts the lexed tokens to an AST (abstract syntax tree) for checking e.g. the "order of operations" of the math equation.
- [X] Simple user-friendly CLI with a few basic commands for evaluating simple single line math expressions.

## Missing extras

- [ ] Implement equations with '='. With this you could write f.e '5 + 10 = 20 - 5' and get f.e. 'true' or 'false'. Can later also be used for solving math equations for a specific variable. Also it can later be used to assign variables or custom function definitions.
- [ ] Implement the usage of multiple function arguments with the ',' seperator so more complex functions can be Implemented.
- [ ] Summation function ∑: 'sum(i, n, expr)'. Sum of 'expr' from 'i' to 'n'. 'i' should be usable from inside the 'expr'.
- [ ] Product function ∏(): 'prod(i, n, expr)'. Product of 'expr' from 'i' to 'n'. 'i' should be usable from inside the 'expr'.
- [X] Handling of float values (not like currently with an extra 'Comma' token but with a different number type literal in the unit f.e.)
- [X] Arena Allocator implementation in the parser for easy memory management
- [ ] Variables and variable assignments and storing variables while running for e.g. multi line math expressions. Rethink if variable assignments should use the '=' literal or a custom assignment symbol like ':='.
- [ ] Solving math expressions with variables and assignments.
- [ ] Custom functions like variables that take one or multiple parameters/variables. These need to be checked for duplicates and also if it is a standard function like f.e 'sqrt'.
- [ ] Implement file handling for writing simple to complex multi line math expressions in a seperate file. This could be used like a programming language for math expressions or equations with variable assignments and evaluations. Maybe the file could get parsed and all the equations and variables get evaluated and stored in memory. The program keeps running in a special mode where you can write a variable name and get its evaluated value. Another possibility could be to use variable and function assignments for handling more complex expressions and only allow 1 evaluation per file. So just 1 complete expression per file and to evaluate the final expression you write '= EXPRESSION WITH FUNCTIONS AND VARIABLES' at the end of the file.
- [ ] Implement new-line capabilities for files to break up a long math expression or f.e. function assignment into multiple lines. For this the '\' literal could be used like in c makros. The definitions would work exactly the same way as in a single line but it is easier to read and use in files. This should only work in files not in the cli.
- [ ] Complex CLI interface with the possibility to enter multiple math expressions without having to restart the program. Here you could run the program and the program starts in full mode and expects a math expression per entered line. This goes hand in hand with the file parsing because it should work exactly the same. You just enter each line while it is running and get instant parsing errors if there are any.
- [ ] Implement comments for files. These should only work in file mode. Comments would look like '... // COMMENT GOES HERE AND TAKES THE REST OF THE LINE.'.
- [ ] Implement a linker, so often used functions can be pre written in one or multiple files, which only contain variable- and function-definitions for easy reusability. This would make it possible to use more complex and custom variables and functions in f.e single-mode where only the fewest features are active. The variables can also be used because they would work exactly the same way as the pre defined math-constants. Same with functions, because they also work the same way like the pre defined functions. The linking could be used with a specific cli command like '-l <FILE>' or '--link <FILE>' and could allow multiple calls for linking one or many different files. The linker than loads everything in memory and pre parses all definitions, so that the user input can be combined like it would be just a single file.

## Standard Math-Functions

All the available math-functions in "math.c". Some might need reconsiderations before implementation.

- [X] acos(x): Returns the arc cosine of x in radians.
- [X] asin(x): Returns the arc sine of x in radians.
- [X] atan(x): Returns the arc tangent of x in radians.
- [ ] atan2(y, x): Returns the arc tangent in radians of y/x based on the signs of both values to determine the correct quadrant.
- [X] cos(x): Returns the cosine of a radian angle x.
- [X] cosh(x): Returns the hyperbolic cosine of x.
- [X] sin(x): Returns the sine of a radian angle x.
- [X] sinh(x): Returns the hyperbolic sine of x.
- [X] tan(x): Returns the tangent of a given angle x.
- [X] tanh(x): Returns the hyperbolic tangent of x.
- [X] exp(x): Returns the value of e raised to the x'th power.
- [ ] ldexp(x, exponent: int): Returns x multiplied by 2 raised to the power of exponent.
- [X] log(x): Returns the natural logarithm (base-e logarithm) of x.
- [X] log10(x): Returns the common logarithm (base-10 logarithm) of x.
- [X] sqrt(x): Returns the square root of x.
