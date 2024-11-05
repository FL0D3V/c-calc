# TODO

[X] - Tokenizer: Converts the input string to valid tokens and checks for invalid characters. Input string must be e.g.: "100.53 + 23 * (53.21 - 34) / 2.5".
[X] - Lexer: Is an extension to the tokenizer which adds additional meta data to the tokens like what type of token and extra values like converted value literal or operator type.
[ ] - Parser: Converts the lexed tokens to an AST (abstract syntax tree) for checking e.g. the "order of operations" of the math equation.
[ ] - Arena Allocator (if needed) for memory management
[ ] - Executing the parsed AST.
