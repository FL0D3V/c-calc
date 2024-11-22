# c-calc

A simple terminal calculator written in C that can parse a given mathematical expression and evaluate it according to the rules of mathematics.


## Math rules

### Order of operations

![PEMDAS](https://static.qumath.in/static/website/old-cdn-static/gurpreet-numbers-seo-03-1614774781.png)


## Whats supported?

**Supported operators:** **+**, **-**, **\***, **/**, **^**

**Supported functions:**
| **Usage** | **Description** |
|--|--|
| **sqrt(x)** | Returns the square root of x. |
| **exp(x)**  | Returns the value of e raised to the x'th power. |
| **sin(x)** | Returns the sine of a radian angle x. |
| **asin(x)** | Returns the arc sine of x in radians. |
| **sinh(x)** | Returns the hyperbolic sine of x. |
| **cos(x)** | Returns the cosine of a radian angle x. |
| **acos(x)** | Returns the arc cosine of x in radians. |
| **cosh(x)** | Returns the hyperbolic cosine of x. |
| **tan(x)** | Returns the tangent of a given angle x. |
| **atan(x)** | Returns the arc tangent of x in radians. |
| **tanh(x)** | Returns the hyperbolic tangent of x. |
| **ln(x)** | Returns the natural logarithm (base-e logarithm) of x. |
| **log10(x)** | Returns the common logarithm (base-10 logarithm) of x. |

**Implemented constants:**
- Pi
- Tau
- Phi
- Euler's number
- Euler's constant
- Omega constant
- Gauss's constant


## Verbose execution example

First 'make' the project and then run the following command for a simple test:

**Input**:
```
ccalc -vv "  100.53 + sqrt(3.5 - EN) + cos(44.23 *   6.4^2) /   8.3 + ln(10) - PI + ln(5^EC)"
```

**Tokenizer Output:**
```
Printing tokenenized input ('33' tokens):
Token('100.53')
Token('+')
Token('sqrt')
Token('(')
Token('3.5')
Token('-')
Token('EN')
Token(')')
Token('+')
Token('cos')
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
Printing lexed tokens ('33' tokens):
Number(100.53000)
Operator(Add)
Function(sqrt, Square-Root)
Parenthesis(Open)
Number(3.50000)
Operator(Subtract)
Constant(2.71828, Euler's number)
Parenthesis(Close)
Operator(Add)
Function(cos, Cosinus)
Parenthesis(Open)
Number(44.23000)
Operator(Multiply)
Number(6.40000)
Operator(Pow)
Number(2.00000)
Parenthesis(Close)
Operator(Divide)
Number(8.30000)
Operator(Add)
Function(ln, Natural-Logarithm)
Parenthesis(Open)
Number(10.00000)
Parenthesis(Close)
Operator(Subtract)
Constant(3.14159, Pi)
Operator(Add)
Function(ln, Natural-Logarithm)
Parenthesis(Open)
Number(5.00000)
Operator(Pow)
Constant(0.57722, Euler's constant)
Parenthesis(Close)
```

**Parsed AST:**
```
add(
  100.53000,
  add(
    sqrt(
      substract(
        3.50000,
        2.71828
      )
    ),
    add(
      divide(
        cos(
          multiply(
            44.23000,
            pow(
              6.40000,
              2.00000
            )
          )
        ),
        8.30000
      ),
      substract(
        ln(
          10.00000
        ),
        add(
          3.14159,
          ln(
            pow(
              5.00000,
              0.57722
            )
          )
        )
      )
    )
  )
)
```

**Evaluated result:**
```
= 99.58497
```


## build

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
