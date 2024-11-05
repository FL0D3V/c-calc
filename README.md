# c-types

A simple calculator implemented in c which parses a math function and tries to execute it.

**Example input:**
- '100.53 * 12.5 + (62.2 - 12.5) / 8'
  This should get tokenized, lexed, parsed and in the execution calculated. Then it should print the answer: '1262.8'



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
