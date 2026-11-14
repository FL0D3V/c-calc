# c-calc

A simple linux terminal calculator written in C which can parse a given mathematical expression and evaluate it.

## Math rules

### Order of operations

![PEMDAS](https://static.qumath.in/static/website/old-cdn-static/gurpreet-numbers-seo-03-1614774781.png)


## Whats supported?

**Supported operators:** **+**, **-**, **\***, **/**, **^**
| **Operator** | **Precedence** | **Description** |
|--|--|
| **+** | 0 | Addition |
| **-** | 0 | Substraction |
| **\*** | 1 | Multiplication |
| **/** | 1 | Division |
| **^** | 2 | Power |

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
| **atan2(x,y)** | Returns the angle theta from the conversion of rectangular coordinates (x, y) to polar coordinates (r, theta). |
| **ln(x)** | Returns the natural logarithm (base-e logarithm) of x. |
| **log10(x)** | Returns the common logarithm (base-10 logarithm) of x. |

**Implemented constants:**
| **Usage** | **Description** |
|--|--|
| **PI** | Pi |
| **TAU** | Tau |
| **PHI** | Phi |
| **EN** | Euler's number |
| **EC** | Euler's constant |
| **OC** | Omega constant |
| **GC** | Gauss's constant |


## Variable Definitions

```
test = 10 + 5 * cos(PI * 1.5)
```
A constant variable definition.
```
test(x, y) = 10 + x * cos(PI * y)
```
A function definition.


## Verbose execution example

First 'make' the project and then run the following command for a simple test:

**Input**:
```
ccalc input -v -i "  100.53 + sqrt(3.5 - EN) + cos(44.23 *   6.4^2) /   8.3 + ln(10) - PI + ln(5^EC)"
```

**Tokenizer Output:**
```
Printing tokenenized input (33 tokens):
Identifier('100.53'), len=6
Literal('+'), len=1
Identifier('sqrt'), len=4
Literal('('), len=1
Identifier('3.5'), len=3
Literal('-'), len=1
Identifier('EN'), len=2
Literal(')'), len=1
Literal('+'), len=1
Identifier('cos'), len=3
Literal('('), len=1
Identifier('44.23'), len=5
Literal('*'), len=1
Identifier('6.4'), len=3
Literal('^'), len=1
Identifier('2'), len=1
Literal(')'), len=1
Literal('/'), len=1
Identifier('8.3'), len=3
Literal('+'), len=1
Identifier('ln'), len=2
Literal('('), len=1
Identifier('10'), len=2
Literal(')'), len=1
Literal('-'), len=1
Identifier('PI'), len=2
Literal('+'), len=1
Identifier('ln'), len=2
Literal('('), len=1
Identifier('5'), len=1
Literal('^'), len=1
Identifier('EC'), len=2
Literal(')'), len=1
```

**Lexer Output:**
```
Printing lexed tokens (34 tokens):
Number(100.53000)
Operator(add): precedence: 0, right-associative: false
Symbol('sqrt'): Function(Square-Root)
Parenthesis(Open)
Number(3.50000)
Operator(subtract): precedence: 0, right-associative: false
Symbol('EN'): Math-Constant(2.71828, Euler's number)
Parenthesis(Close)
Operator(add): precedence: 0, right-associative: false
Symbol('cos'): Function(Cosinus)
Parenthesis(Open)
Number(44.23000)
Operator(multiply): precedence: 1, right-associative: false
Number(6.40000)
Operator(pow): precedence: 2, right-associative: true
Number(2.00000)
Parenthesis(Close)
Operator(divide): precedence: 1, right-associative: false
Number(8.30000)
Operator(add): precedence: 0, right-associative: false
Symbol('ln'): Function(Natural-Logarithm)
Parenthesis(Open)
Number(10.00000)
Parenthesis(Close)
Operator(subtract): precedence: 0, right-associative: false
Symbol('PI'): Math-Constant(3.14159, Pi)
Operator(add): precedence: 0, right-associative: false
Symbol('ln'): Function(Natural-Logarithm)
Parenthesis(Open)
Number(5.00000)
Operator(pow): precedence: 2, right-associative: true
Symbol('EC'): Math-Constant(0.57722, Euler's constant)
Parenthesis(Close)
EndOfFile
```

**Parsed AST:**
```
add(
  add(
    add(
      100.53000,
      sqrt(
        subtract(
          3.50000,
          EN (2.71828, Euler's number)
        )
      )
    ),
    divide(
      cos(
        pow(
          multiply(
            44.23000,
            6.40000
          ),
          2.00000
        )
      ),
      8.30000
    )
  ),
  add(
    subtract(
      ln(10.00000),
      PI (3.14159, Pi)
    ),
    ln(
      pow(
        5.00000,
        EC (0.57722, Euler's constant)
      )
    )
  )
)
```

**Evaluated result:**
```
>>> 101.61941
```


## build

Just run the following commands to build and test the demo.

```
make | make debug | make release
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
make remake | remakeDebug | remakeRelease
```
This command tries to clean the project and then builds it again.
```
make check[1-5]
```
Runs 'valgrind' to check for memory leaks.
```
make test[1-2]
```
Runs premade tests.
