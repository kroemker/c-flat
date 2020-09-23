# c-flat
C♭ is an embeddable stack-based scripting language based on C. It is aimed at tasks that require a sandbox environment but still need access to low-level bit manipulations.
C♭ code is compiled into byte code instructions that operate solely on the stack.

## Features
* fully stack-based
* easily embeddable
* C-style syntax
* strictly typed
* bit operations
* supports recursion

The following C♭ function computes the n-th fibonacci number (see [examples](examples/)):
```
function fib(int n)
{
    if (n <= 0) 
        return 0;
    else if (n == 1) 
        return 1;
    else 
        return fib(n-1) + fib(n-2);
}
```

## Building
The makefile automatically detects Windows and Linux platforms. 
To build and install the language for a user:

```
make all && make install
```

Or install locally in the repo directory:

```
make all && make local
```

## Usage
The language comes with an interpreter but can be easily integrated in other software projects. 
While an example of executing C♭ scripts can be seen in the [interpreter source](src/icflat.cpp), the main steps will be shown in the following:

1. Link the library with `-lcflat` and include the header: `#include <cflat.h>`
1. Start the C♭ environment: `cfOpen()`
2. Register your own functions to be used by C♭ code: `cfRegisterFunction("name", FunctionPtr)`
3. Parse the C♭ file to execute: `cfParseFile("filename")`
4. Initialize the stack for execution: `cfInitStack(size)`
5. Execute
   - script by invoking a main function: `cfExecute("main")`
   - function with arguments: `cfCallFunction("functionName", nargs, ...)`
6. Retrieve the return value (only possible if invoking a function): `cfGetReturnInteger()` or `cfGetReturnFloat()`
7. Close the C♭ environment: `cfClose()`

