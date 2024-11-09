# c-types

A simple calculator implemented in c which can parse a math expression and execute it.
It also checks the order of operation explained in the following picture.

![PEMDAS](https://static.qumath.in/static/website/old-cdn-static/gurpreet-numbers-seo-03-1614774781.png)

**This calculator currently supports:**
- Decimal number input
- Operators: +, -, *, /, ^
- Brackets: (, )
- Math constants: Pi, Tau, Phi, Euler's number, Euler's constant, Omega constant, Gauss's constant
- Functions: sqrt, sin, cos, tan, ln


## Verbose execution example

First 'make' the project and then run the following command for a simple test:

**Input**:
```
ccalc -vv "  100.53 + sqrt(3.5 - EN) + (44.23 *   6.4^2) /   8.3 + ln(10) - PI + ln(5^EC)"
```

**Tokenizer Output:**
```
Printing tokenenized input ('32' tokens):
Token('100.53')
Token('+')
Token('sqrt')
Token('(')
Token('3.5')
Token('-')
Token('EN')
Token(')')
Token('+')
Token('(')
Token('44.23')
Token('*')
Token('6.4')
Token('^')
Token('2')
Token(')')
Token('/')
Token('8.3')
Token('+')
Token('ln')
Token('(')
Token('10')
Token(')')
Token('-')
Token('PI')
Token('+')
Token('ln')
Token('(')
Token('5')
Token('^')
Token('EC')
Token(')')
```

**Lexer Output:**
```
Printing lexed tokens ('32' tokens):
Number(100.53000)
Operator(Add)
Function(sqrt)
Bracket(Open-Paren)
Number(3.50000)
Operator(Subtract)
Constant(2.71828183, Euler's number)
Bracket(Closing-Paren)
Operator(Add)
Bracket(Open-Paren)
Number(44.23000)
Operator(Multiply)
Number(6.40000)
Operator(Pow)
Number(2.00000)
Bracket(Closing-Paren)
Operator(Divide)
Number(8.30000)
Operator(Add)
Function(ln)
Bracket(Open-Paren)
Number(10.00000)
Bracket(Closing-Paren)
Operator(Subtract)
Constant(3.14159265, Pi)
Operator(Add)
Function(ln)
Bracket(Open-Paren)
Number(5.00000)
Operator(Pow)
Constant(0.57721566, Euler's constant)
Bracket(Closing-Paren)
```


## Example

**Input:**
- '100.53 * 12.5 + (62.2 - 12.5) / 8'

This should get tokenized, lexed, parsed and in the execution calculated.

**Output:**
- '1262.8'


# build

Just run the following commands to build and test the demo.

```
make
```
Tries to build the project.
```
make run
```
Runs the project if already build else it builds it and then runs it automatically.
```
make clean
```
Cleans the project from all build/object files.
```
make remake
```
This command tries to clean the project and then it builds it again.
```
make check
```
Runs 'valgrind' to check for memory leaks.
