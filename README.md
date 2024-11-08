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


## Tokenizer and Lexer Output

First 'make' the project and then run the following command for a simple test:

**Input**:
```
./bin/ccalc "  100.53 + sqrt(3.5 - EN) + (44.23 *   6.4^2) /   8.3 + ln(10) - PI "
```

**Tokenizer Output:**
```
Printing tokenenized input ('25' tokens found):
TOKEN('100.53'); CURSOR(2)
TOKEN('+'); CURSOR(8)
TOKEN('sqrt'); CURSOR(11)
TOKEN('('); CURSOR(14)
TOKEN('3.5'); CURSOR(16)
TOKEN('-'); CURSOR(19)
TOKEN('EN'); CURSOR(22)
TOKEN(')'); CURSOR(23)
TOKEN('+'); CURSOR(25)
TOKEN('('); CURSOR(27)
TOKEN('44.23'); CURSOR(29)
TOKEN('*'); CURSOR(34)
TOKEN('6.4'); CURSOR(39)
TOKEN('^'); CURSOR(41)
TOKEN('2'); CURSOR(43)
TOKEN(')'); CURSOR(43)
TOKEN('/'); CURSOR(45)
TOKEN('8.3'); CURSOR(50)
TOKEN('+'); CURSOR(53)
TOKEN('ln'); CURSOR(56)
TOKEN('('); CURSOR(57)
TOKEN('10'); CURSOR(59)
TOKEN(')'); CURSOR(60)
TOKEN('-'); CURSOR(62)
TOKEN('PI'); CURSOR(65)
```

**Lexer Output:**
```
Printing lexed tokens ('25' tokens found):
Literal(100.5300)
Operator(Add)
Function(sqrt)
Bracket(Open-Paren)
Literal(3.5000)
Operator(Subtract)
MathConstant(Euler's number)
Bracket(Closing-Paren)
Operator(Add)
Bracket(Open-Paren)
Literal(44.2300)
Operator(Multiply)
Literal(6.4000)
Operator(Pow)
Literal(2.0000)
Bracket(Closing-Paren)
Operator(Divide)
Literal(8.3000)
Operator(Add)
Function(ln)
Bracket(Open-Paren)
Literal(10.0000)
Bracket(Closing-Paren)
Operator(Subtract)
MathConstant(Pi)
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
