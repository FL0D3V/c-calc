# c-types

A simple calculator implemented in c which can parse a math expression and execute it.
It also checks the order of operation explained in the following picture.

[PEMDAS](https://static.qumath.in/static/website/old-cdn-static/gurpreet-numbers-seo-03-1614774781.png)


## Tokenizer and Lexer Output

First 'make' the project and then run the following command for a simple test:

**Input**:
```
./bin/ccalc "  100.53 + Sqrt(3.5 - 5) + (44.23 *   6.4) /   8.3  "
```

**Tokenizer Output:**
```
Printing tokenenized input ('16' tokens found):
1: TOKEN('100.53'); CURSOR(2)
2: TOKEN('+'); CURSOR(8)
3: TOKEN('Sqrt'); CURSOR(11)
4: TOKEN('('); CURSOR(14)
5: TOKEN('3.5'); CURSOR(16)
6: TOKEN('-'); CURSOR(19)
7: TOKEN('5'); CURSOR(22)
8: TOKEN(')'); CURSOR(22)
9: TOKEN('+'); CURSOR(24)
10: TOKEN('('); CURSOR(26)
11: TOKEN('44.23'); CURSOR(28)
12: TOKEN('*'); CURSOR(33)
13: TOKEN('6.4'); CURSOR(38)
14: TOKEN(')'); CURSOR(40)
15: TOKEN('/'); CURSOR(42)
16: TOKEN('8.3'); CURSOR(47)
```

**Lexer Output:**
```
Printing lexed tokens ('16' tokens found):
Literal(100.5300)
Operator(Add)
Function(Sqrt)
Bracket(Open-Paren)
Literal(3.5000)
Operator(Subtract)
Literal(5.0000)
Bracket(Closing-Paren)
Operator(Add)
Bracket(Open-Paren)
Literal(44.2300)
Operator(Multiply)
Literal(6.4000)
Bracket(Closing-Paren)
Operator(Divide)
Literal(8.3000)
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
