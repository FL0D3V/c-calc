# TODO

## Main Funcionality

- [X] Tokenizer: Converts the input string to valid tokens and checks for invalid characters. Input string must be e.g.: "100.53 + 23 * (53.21 - 34) / 2.5".
- [X] Lexer: Is an extension to the tokenizer which adds additional meta data to the tokens like what type of token and extra values like converted value literal or operator type.
- [ ] Parser: Converts the lexed tokens to an AST (abstract syntax tree) for checking e.g. the "order of operations" of the math equation.
- [ ] Executing the parsed AST.
- [X] Simple user-friendly CLI.
- [ ] Complex CLI interface with the possibility to enter multiple math expressions without having to restart the program.

## Missing Extras

- [X] 'Peek' and 'Seek' functions for the tokenization (not really needed with current implementation of it but propably needed for better tokenization)
- [X] Handling of float values (not like currently with an extra 'Comma' token but with a different number type literal in the unit f.e.)
- [ ] Arena Allocator (if needed) for memory management
- [ ] Variables and variable assignments and storing variables while running for e.g. multi line math expressions.
- [ ] Solving math expressions with variables and assignments.
