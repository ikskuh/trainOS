# Trainscript Version 1 Specification

## Basic Language Features

The language is not case sensitive.

Keywords should be written **CAPITALIZED**.

Identifieres should be written **lowercased**. 

### Comments
Comments are written with a starting #

    normal code
    # comment
    normal code # comment

### Types
The language has the following built in primitive types:

- VOID
- BOOL
- INT
- REAL
- TEXT

`VOID` is a special type that can only be used in function
return values indicating the function does not return anything

### Pointers
The language supports pointers by using special operators.
Pointers can exist to every type the language supports, not
including pointers.

### Blocks 
Blocks are defined by indentation in this language. An
indentation by a 2 spaces is required:

    blockA
      blockB
      blockB
        blockC
        blockC
      blockb
    blockA
    blockA

## Grammarless Description

### Variable Declaration

    VAR name : type;

### Expressions And Assignments

An expression is a piece of code that calculates a value.
An assignment assigns the result of an expression to a
variable. The assignment is either done by `->` or `→`.

    1 → a;
    a → b;
    a + b → c;
    a * (b + c) → d;

### Control Structures

The language supports the most basic control structures:

- IF
- IF/ELSE
- IF/ELSEIF/ELSE
- REPEAT
- REPEAT WHILE
- REPEAT UNTIL
- REPEAT FROM TO

Each control structure except `REPEAT` takes an expression
for defining the control. The expression is executed once or
multiple times depending on the behaviour of the control
structure.

    # Simple condition
    IF expr
      loc;
    
    # Condition with alternative
    IF expr
      loc;
    ELSE
      loc;
    
    # Condition with multiple alternatives
    IF expr
      loc;
    ELSEIF expr2 
      loc;
    
    IF expr
      loc;
    ELSEIF expr2
      loc;
    ELSE
      loc;
    
    # Endless loop
    REPEAT
      loc;
    
    # "While" loop
    REPEAT WHILE expr
      loc;
    
    # Inverted "Do-While" loop
    REPEAT UNTIL expr
      loc;
    
    # "For" loop
    REPEAT a FROM 1 TO 10
      loc;
    
### Function Calls

Every function returns a value and thus can be included in an expression.
Functions calls do not have to be in an expression and can discard the return value.

    fn();
    fn(a);
    fn(a,b);
    fn() → a;
    fn(a) → b;
    fn(a,b) → c;

### Function Declaration

Functions are declared by using either the `PUB` or the `PRI` keyword.
Each script file can be seen as a module that exports a set of functions.
`PUB` declares a function that can be seen from outside the module, `PRI`
declares a hidden function.

Each function has a set of local variables that have a specific type.

Returning a value is done by declaring a special local variable that can
be named as wanted.

    # Program that returns 0
    PUB main() → result : INT
        0 → result;

    PUB noReturn()
        otherFn();
    
    # Private function that doubles the given value squared.
    PRI fn(x : REAL) → y : REAL
        2 * x * x → y;


### Pointer Declaration And Usage

A pointer is declared by using `PTR(type)` as the type.

    VAR name : PTR(INT);

To retrieve a pointer value or write to it, `VAL(pointer)` must be used:

    VAL(pointer) → a;
    a → VAL(pointer);

The address of a variable can be retrieved by using `REF(variable)`:

    REF(variable) → pointer;

## Example File

    PUB inOut() | a : INT
      printStr("Enter a number:");
      readInt() → a;
      printStr("You entered: ");
      printInt(a);
      printLn(); # Prints a newline ('\n')

    PUB isEven(x : INT) → even : BOOL
      IF (x % 2) =/= 0
        false → even
      ELSE
        true → even
    
    PUB fibonacci(n : INT) → f : INT
      IF n = 0
        0 → f;
      ELSEIF n = 1
        1 → f;
      ELSE
        fibonacci(n - 1) + fibonacci(n - 2) → f;
    
    PUB factorial(number : INT) → result : INT
      IF number <= 1
        1 → result;
      ELSE 
        number * factorial(number - 1) → result;
    
    PRI double(ptr : PTR(INT)) → previous : INT
      VAL(ptr) → previous;
      2 * previous → VAL(ptr);
    
    PUB ptrTest() | a : INT, b : INT
      10 → a;
      double(REF(a)) → b;
      IF a = 20
        printStr("double has doubled a.");
      IF a = (2 * b)
        printStr("double has returned the correct previous value");
      